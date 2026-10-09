#include "indoru/planet_atmosphere.hpp"

#include <algorithm>
#include <cmath>

namespace indoru::atmosphere {
namespace {
constexpr double Pi = 3.14159265358979323846;
constexpr double DegreesToRadians = Pi / 180.0;
constexpr double R = 287.05;
constexpr double Gravity = 9.80665;

bool finite_non_negative(double value) noexcept {
    return std::isfinite(value) && value >= 0.0;
}

double clamp(double value, double low, double high) noexcept {
    return std::max(low, std::min(high, value));
}

double wave(double latitude, double longitude, double time, std::uint64_t seed) noexcept {
    const double seedPhase = static_cast<double>((seed % 10'000U)) * 0.00037;
    return 0.5 + 0.5 * std::sin(latitude * 2.7 + longitude * 1.9 + time * 0.000018 + seedPhase);
}
}

bool valid_config(const Config& config) noexcept {
    return std::isfinite(config.seaLevelTemperatureC) && std::isfinite(config.poleTemperatureDropC) &&
           finite_non_negative(config.seasonalTemperatureAmplitudeC) && std::isfinite(config.seaLevelPressurePa) &&
           config.seaLevelPressurePa > 1.0 && std::isfinite(config.scaleHeightMeters) && config.scaleHeightMeters > 100.0 &&
           std::isfinite(config.maximumWindMetersPerSecond) && config.maximumWindMetersPerSecond > 0.0;
}

bool valid_forcing(const Forcing& forcing) noexcept {
    return std::isfinite(forcing.simulationSeconds) && std::isfinite(forcing.orbitalPhaseRadians) &&
           std::isfinite(forcing.terrainElevationMeters) && std::isfinite(forcing.weatherIntensity) &&
           forcing.weatherIntensity >= 0.0 && forcing.weatherIntensity <= 1.0;
}

State sample(const Config& config, const Forcing& forcing, globe::LatLon location) noexcept {
    State state;
    if (!valid_config(config) || !valid_forcing(forcing) || !globe::valid(location)) return state;
    location = globe::normalize(location);
    const double latitude = location.latitudeDegrees * DegreesToRadians;
    const double longitude = location.longitudeDegrees * DegreesToRadians;
    const double latitudeAbs = std::abs(location.latitudeDegrees) / 90.0;
    const double spatialWave = wave(latitude, longitude, forcing.simulationSeconds, config.seed);
    const double dailyPhase = forcing.simulationSeconds * 2.0 * Pi / 86'400.0;
    const double seasonal = std::cos(latitude) * std::sin(forcing.orbitalPhaseRadians) * config.seasonalTemperatureAmplitudeC;

    state.temperatureC = config.seaLevelTemperatureC - latitudeAbs * config.poleTemperatureDropC + seasonal;
    state.temperatureC += (spatialWave - 0.5) * 7.0;
    if (forcing.overOcean) state.temperatureC += 2.0 * std::cos(latitude);
    state.temperatureC -= std::max(0.0, forcing.terrainElevationMeters) * 0.0065;

    const double pressureRatio = std::exp(-std::max(0.0, forcing.terrainElevationMeters) / config.scaleHeightMeters);
    state.pressurePa = config.seaLevelPressurePa * pressureRatio;
    state.pressurePa += (spatialWave - 0.5) * 2'400.0;
    state.pressurePa = std::max(100.0, state.pressurePa);

    state.relativeHumidity = clamp(0.50 + 0.18 * std::cos(latitude) + (spatialWave - 0.5) * 0.22 +
                                       (forcing.overOcean ? 0.15 : 0.0) + forcing.weatherIntensity * 0.12,
                                   0.05, 1.0);
    state.cloudFactor = clamp(state.relativeHumidity * 0.85 + forcing.weatherIntensity * 0.25, 0.0, 1.0);
    state.precipitationMillimetersPerHour = state.cloudFactor * state.cloudFactor *
                                             (0.4 + forcing.weatherIntensity * 12.0);

    const double zonal = std::sin(latitude * 3.0 + dailyPhase) * 5.5 + (spatialWave - 0.5) * 8.0;
    const double meridional = std::sin(longitude * 2.0 - dailyPhase * 0.7) * 3.0 +
                              std::cos(latitude * 2.0) * (spatialWave - 0.5) * 4.0;
    const double gust = 1.0 + forcing.weatherIntensity * 0.75;
    state.windEastMetersPerSecond = zonal * gust;
    state.windNorthMetersPerSecond = meridional * gust;
    state.windUpMetersPerSecond = (state.cloudFactor - 0.5) * 1.5;
    const double horizontalSpeed = std::hypot(state.windEastMetersPerSecond, state.windNorthMetersPerSecond);
    state.windSpeedMetersPerSecond = std::min(config.maximumWindMetersPerSecond,
                                              std::hypot(horizontalSpeed, state.windUpMetersPerSecond));
    if (state.windSpeedMetersPerSecond > 1e-9) {
        state.windDirectionDegrees = std::fmod(std::atan2(state.windEastMetersPerSecond,
                                                          state.windNorthMetersPerSecond) / DegreesToRadians + 360.0,
                                               360.0);
    }
    state.airDensityKgPerM3 = state.pressurePa / (R * (state.temperatureC + 273.15));
    if (!std::isfinite(state.airDensityKgPerM3) || state.airDensityKgPerM3 <= 0.0) return State{};
    return state;
}

} // namespace indoru::atmosphere
