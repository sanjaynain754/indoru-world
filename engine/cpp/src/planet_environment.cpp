#include "indoru/planet_environment.hpp"

#include <algorithm>
#include <cmath>

namespace indoru::environment {
namespace {
constexpr double Pi = 3.14159265358979323846;

// earth::sample accepts a bounded gameplay altitude; deeper bathymetry is
// clamped rather than rejected so every terrain tile still yields gravity.
constexpr double MinimumGravityAltitudeMeters = -1'000.0;
constexpr double MaximumGravityAltitudeMeters = 100'000.0;

double clamp(double value, double low, double high) noexcept {
    return std::max(low, std::min(high, value));
}

bool finite_non_negative(double value) noexcept {
    return std::isfinite(value) && value >= 0.0;
}
} // namespace

bool valid_config(const Config& config) noexcept {
    return planet::valid_clock(config.clock) && terrain::valid_config(config.terrain) &&
           atmosphere::valid_config(config.atmosphere) && ocean::valid_config(config.ocean) &&
           sky::valid_config(config.sky) && finite_non_negative(config.waveFetchMeters) &&
           finite_non_negative(config.swellHeightMeters) && std::isfinite(config.swellPeriodSeconds) &&
           config.swellPeriodSeconds >= 2.0 && config.swellPeriodSeconds <= 30.0 &&
           finite_non_negative(config.seabedDepthOffsetMeters);
}

bool valid_forcing(const Forcing& forcing) noexcept {
    return std::isfinite(forcing.simulationSeconds) && std::isfinite(forcing.weatherIntensity) &&
           forcing.weatherIntensity >= 0.0 && forcing.weatherIntensity <= 1.0 &&
           std::isfinite(forcing.tidePhaseRadians);
}

State sample(const Config& config, const Forcing& forcing, globe::LatLon location) noexcept {
    State state;
    if (!valid_config(config) || !valid_forcing(forcing) || !globe::valid(location)) return state;
    location = globe::normalize(location);
    state.location = location;

    state.ground = terrain::sample(config.terrain, location);
    const double altitude = clamp(state.ground.elevationMeters,
                                  MinimumGravityAltitudeMeters,
                                  MaximumGravityAltitudeMeters);
    state.gravity = earth::sample(location.latitudeDegrees, location.longitudeDegrees, altitude);
    if (state.gravity.accelerationMetersPerSecondSquared <= 0.0) return State{};

    planet::Clock clock = config.clock;
    clock.simulationSeconds = forcing.simulationSeconds;
    if (!planet::valid_clock(clock)) return State{};
    state.sun = planet::sample_sun(location, clock);
    const double orbitalPhaseRadians =
        2.0 * Pi * std::fmod(forcing.simulationSeconds, clock.orbitalPeriodSeconds) / clock.orbitalPeriodSeconds;

    atmosphere::Forcing airForcing{};
    airForcing.simulationSeconds = forcing.simulationSeconds;
    airForcing.orbitalPhaseRadians = orbitalPhaseRadians;
    airForcing.terrainElevationMeters = state.ground.elevationMeters;
    airForcing.weatherIntensity = forcing.weatherIntensity;
    airForcing.overOcean = state.ground.ocean;
    state.air = atmosphere::sample(config.atmosphere, airForcing, location);

    sky::Forcing skyForcing{};
    skyForcing.sun = state.sun;
    skyForcing.air = state.air;
    skyForcing.simulationSeconds = forcing.simulationSeconds;
    skyForcing.observerAltitudeMeters = state.ground.elevationMeters;
    skyForcing.weatherIntensity = forcing.weatherIntensity;
    state.sky = sky::sample(config.sky, skyForcing, location);

    ocean::Forcing waterForcing{};
    waterForcing.simulationSeconds = forcing.simulationSeconds;
    waterForcing.windSpeedMetersPerSecond = state.air.windSpeedMetersPerSecond;
    waterForcing.windDirectionDegrees = state.air.windDirectionDegrees;
    waterForcing.fetchMeters = config.waveFetchMeters;
    waterForcing.swellHeightMeters = config.swellHeightMeters;
    waterForcing.swellPeriodSeconds = config.swellPeriodSeconds;
    waterForcing.tidePhaseRadians = forcing.tidePhaseRadians;
    waterForcing.seasonalPhaseRadians = orbitalPhaseRadians;

    ocean::CoastSample coast{};
    coast.location = location;
    coast.terrainElevationMeters = state.ground.elevationMeters;
    coast.seabedDepthMeters = config.seabedDepthOffsetMeters;
    coast.beachSlopeDegrees = clamp(state.ground.slopeDegrees, 0.0, 90.0);
    state.water = ocean::sample(config.ocean, waterForcing, coast);

    state.valid = true;
    return state;
}

} // namespace indoru::environment
