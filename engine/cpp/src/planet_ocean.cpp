#include "indoru/planet_ocean.hpp"

#include <algorithm>
#include <cmath>

namespace indoru::ocean {
namespace {
constexpr double Pi = 3.14159265358979323846;
constexpr double EarthRadiusMeters = 6'378'137.0;
constexpr double MinimumWavePeriodSeconds = 2.0;
constexpr double MaximumWavePeriodSeconds = 30.0;

bool finite_non_negative(double value) noexcept {
    return std::isfinite(value) && value >= 0.0;
}

double clamp(double value, double low, double high) noexcept {
    return std::max(low, std::min(high, value));
}

double radians(double degrees) noexcept { return degrees * Pi / 180.0; }

double wrap_radians(double value) noexcept {
    value = std::fmod(value + Pi, 2.0 * Pi);
    if (value < 0.0) value += 2.0 * Pi;
    return value - Pi;
}

double tide_level(const Config& config, const Forcing& forcing) noexcept {
    const double phase = 2.0 * Pi * forcing.simulationSeconds / config.tidePeriodSeconds + forcing.tidePhaseRadians;
    return 0.5 * config.tidalRangeMeters * std::sin(phase);
}

double fetch_limited_wind_wave_height(double gravity, const Forcing& forcing) noexcept {
    if (forcing.windSpeedMetersPerSecond <= 1e-9 || forcing.fetchMeters <= 1e-9) return 0.0;
    const double windSquared = forcing.windSpeedMetersPerSecond * forcing.windSpeedMetersPerSecond;
    const double fullyDeveloped = 0.21 * windSquared / gravity;
    const double fetchScale = gravity * forcing.fetchMeters / windSquared;
    const double fetchFactor = std::tanh(std::pow(0.0125 * fetchScale, 0.42));
    return fullyDeveloped * fetchFactor;
}

double wave_length(double gravity, double period, double depth) noexcept {
    const double deepWaterLength = gravity * period * period / (2.0 * Pi);
    if (depth <= 0.0) return deepWaterLength;
    const double shallowCorrection = std::tanh(2.0 * Pi * depth / deepWaterLength);
    return std::max(0.1, deepWaterLength * shallowCorrection);
}

double synthetic_noise(double latitude, double longitude, double time, std::uint64_t seed) noexcept {
    const double phase = static_cast<double>(seed % 65'536U) * 0.00017;
    return std::sin(latitude * 3.1 + longitude * 2.4 + time * 0.000006 + phase);
}

CoastalZone classify_zone(const Config& config, const CoastSample& coast, double waterDepth) noexcept {
    if (waterDepth > 0.0) {
        return waterDepth < config.shallowWaterDepthMeters ? CoastalZone::ShallowSea : CoastalZone::DeepOcean;
    }
    const double absoluteElevation = coast.terrainElevationMeters - config.seaLevelMeters;
    if (absoluteElevation <= config.beachMaxElevationMeters) {
        if (coast.beachSlopeDegrees <= config.beachMaxSlopeDegrees) return CoastalZone::Beach;
        return CoastalZone::TidalFlat;
    }
    return CoastalZone::RockyCoast;
}
} // namespace

bool valid_config(const Config& config) noexcept {
    return std::isfinite(config.seaLevelMeters) && finite_non_negative(config.beachMaxElevationMeters) &&
           std::isfinite(config.beachMaxSlopeDegrees) && config.beachMaxSlopeDegrees >= 0.0 &&
           config.beachMaxSlopeDegrees <= 90.0 && finite_non_negative(config.tidalRangeMeters) &&
           std::isfinite(config.tidePeriodSeconds) && config.tidePeriodSeconds > 0.0 &&
           std::isfinite(config.shallowWaterDepthMeters) && config.shallowWaterDepthMeters > 0.0 &&
           std::isfinite(config.breakingDepthRatio) && config.breakingDepthRatio > 0.0 &&
           config.breakingDepthRatio <= 1.0 && std::isfinite(config.waterDensityKgPerM3) &&
           config.waterDensityKgPerM3 > 0.0 && std::isfinite(config.equatorialTemperatureC) &&
           std::isfinite(config.polarTemperatureC) && config.equatorialTemperatureC > config.polarTemperatureC &&
           finite_non_negative(config.seasonalTemperatureAmplitudeC) && std::isfinite(config.baseSalinityPsu) &&
           config.baseSalinityPsu > 0.0 && std::isfinite(config.maximumCurrentMetersPerSecond) &&
           config.maximumCurrentMetersPerSecond > 0.0;
}

bool valid_forcing(const Forcing& forcing) noexcept {
    return std::isfinite(forcing.simulationSeconds) && finite_non_negative(forcing.windSpeedMetersPerSecond) &&
           std::isfinite(forcing.windDirectionDegrees) && finite_non_negative(forcing.fetchMeters) &&
           finite_non_negative(forcing.swellHeightMeters) && std::isfinite(forcing.swellPeriodSeconds) &&
           forcing.swellPeriodSeconds >= MinimumWavePeriodSeconds &&
           forcing.swellPeriodSeconds <= MaximumWavePeriodSeconds && std::isfinite(forcing.tidePhaseRadians) &&
           std::isfinite(forcing.seasonalPhaseRadians);
}

bool valid_coast_sample(const CoastSample& sample) noexcept {
    return globe::valid(sample.location) && std::isfinite(sample.terrainElevationMeters) &&
           finite_non_negative(sample.seabedDepthMeters) && std::isfinite(sample.beachSlopeDegrees) &&
           sample.beachSlopeDegrees >= 0.0 && sample.beachSlopeDegrees <= 90.0;
}

OceanState sample(const Config& config, const Forcing& forcing, const CoastSample& coast) noexcept {
    OceanState state;
    if (!valid_config(config) || !valid_forcing(forcing) || !valid_coast_sample(coast)) return state;

    state.gravity = earth::sample(coast.location.latitudeDegrees, coast.location.longitudeDegrees);
    state.gravityMetersPerSecondSquared = state.gravity.accelerationMetersPerSecondSquared;
    if (state.gravityMetersPerSecondSquared <= 0.0) return OceanState{};

    state.tideMeters = tide_level(config, forcing);
    const double tideAdjustedSeaLevel = config.seaLevelMeters + state.tideMeters;
    state.waterDepthMeters = std::max(0.0, tideAdjustedSeaLevel - coast.terrainElevationMeters + coast.seabedDepthMeters);
    state.submerged = state.waterDepthMeters > 0.0;
    state.zone = classify_zone(config, coast, state.waterDepthMeters);
    state.surfaceElevationMeters = state.submerged ? tideAdjustedSeaLevel : coast.terrainElevationMeters;

    if (state.submerged) {
        const double latitude = radians(coast.location.latitudeDegrees);
        const double longitude = radians(coast.location.longitudeDegrees);
        const double latitudeBlend = std::pow(std::max(0.0, std::cos(latitude)), 0.65);
        const double noise = synthetic_noise(latitude, longitude, forcing.simulationSeconds, config.syntheticSeed);
        state.waterTemperatureC = config.polarTemperatureC +
                                  (config.equatorialTemperatureC - config.polarTemperatureC) * latitudeBlend +
                                  config.seasonalTemperatureAmplitudeC * std::cos(latitude) *
                                      std::sin(forcing.seasonalPhaseRadians) + noise * 1.5;
        state.salinityPsu = clamp(config.baseSalinityPsu + noise * 0.8 +
                                      0.35 * std::sin(longitude * 2.0 - latitude * 1.5),
                                  28.0, 40.0);
        state.waterDensityKgPerM3 = config.waterDensityKgPerM3 +
                                    (state.salinityPsu - config.baseSalinityPsu) * 0.75 -
                                    (state.waterTemperatureC - 15.0) * 0.2;
        const double gyre = std::sin(latitude * 2.0 + forcing.seasonalPhaseRadians + noise * 0.35);
        const double currentScale = config.maximumCurrentMetersPerSecond *
                                    (0.35 + 0.65 * std::abs(std::cos(latitude)));
        state.currentEastMetersPerSecond = currentScale * gyre;
        state.currentNorthMetersPerSecond = currentScale * 0.35 *
                                            std::sin(longitude * 1.7 - forcing.seasonalPhaseRadians);
    }

    const double windWaveHeight = fetch_limited_wind_wave_height(state.gravityMetersPerSecondSquared, forcing);
    const double swellHeight = forcing.swellHeightMeters;
    const double combinedHeight = std::sqrt(windWaveHeight * windWaveHeight + swellHeight * swellHeight);
    if (!state.submerged || combinedHeight <= 1e-9) {
        state.surfaceVelocityXMetersPerSecond = state.currentEastMetersPerSecond;
        state.surfaceVelocityZMetersPerSecond = state.currentNorthMetersPerSecond;
        return state;
    }

    state.dominantPeriodSeconds = clamp(
        std::max(forcing.swellPeriodSeconds, 7.54 * forcing.windSpeedMetersPerSecond / state.gravityMetersPerSecondSquared),
        MinimumWavePeriodSeconds, MaximumWavePeriodSeconds);
    state.wavelengthMeters = wave_length(state.gravityMetersPerSecondSquared, state.dominantPeriodSeconds, state.waterDepthMeters);
    state.crestSpeedMetersPerSecond = state.wavelengthMeters / state.dominantPeriodSeconds;
    state.significantWaveHeightMeters = combinedHeight;

    const double breakingHeight = config.breakingDepthRatio * state.waterDepthMeters;
    if (state.significantWaveHeightMeters > breakingHeight) {
        state.significantWaveHeightMeters = std::max(0.0, breakingHeight);
        state.breaking = true;
    }
    state.foamFactor = state.breaking ? clamp(state.significantWaveHeightMeters / std::max(0.1, breakingHeight), 0.0, 1.0) : 0.0;

    const double direction = radians(forcing.windDirectionDegrees);
    const double wavePhase = 2.0 * Pi * forcing.simulationSeconds / state.dominantPeriodSeconds +
                             wrap_radians(radians(coast.location.longitudeDegrees) * std::cos(direction) +
                                          radians(coast.location.latitudeDegrees) * std::sin(direction));
    const double amplitude = 0.5 * state.significantWaveHeightMeters;
    state.surfaceElevationMeters += amplitude * std::sin(wavePhase);
    const double velocityAmplitude = amplitude * 2.0 * Pi / state.dominantPeriodSeconds;
    state.surfaceVelocityXMetersPerSecond = state.currentEastMetersPerSecond +
                                            velocityAmplitude * std::cos(wavePhase) * std::cos(direction);
    state.surfaceVelocityZMetersPerSecond = state.currentNorthMetersPerSecond +
                                            velocityAmplitude * std::cos(wavePhase) * std::sin(direction);
    return state;
}

BuoyancyResult buoyancy(const Config& config,
                        const OceanState& state,
                        double displacedVolumeM3,
                        double bodyMassKg,
                        double submergedFraction) noexcept {
    BuoyancyResult result;
    if (!valid_config(config) || !state.submerged || !std::isfinite(state.gravityMetersPerSecondSquared) ||
        state.gravityMetersPerSecondSquared <= 0.0 || !finite_non_negative(displacedVolumeM3) ||
        !std::isfinite(bodyMassKg) || bodyMassKg <= 0.0 || !std::isfinite(submergedFraction)) return result;

    const double fraction = clamp(submergedFraction, 0.0, 1.0);
    const double density = state.waterDensityKgPerM3 > 0.0 ? state.waterDensityKgPerM3 : config.waterDensityKgPerM3;
    result.accelerationMetersPerSecondSquared =
        density * displacedVolumeM3 * state.gravityMetersPerSecondSquared * fraction / bodyMassKg;
    result.acceleration = {
        -state.gravity.downX * result.accelerationMetersPerSecondSquared,
        -state.gravity.downY * result.accelerationMetersPerSecondSquared,
        -state.gravity.downZ * result.accelerationMetersPerSecondSquared,
    };
    return result;
}

} // namespace indoru::ocean
