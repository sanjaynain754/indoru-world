#pragma once

#include "indoru/earth_gravity.hpp"
#include "indoru/globe_projection.hpp"

#include <cstdint>

namespace indoru::ocean {

enum class CoastalZone : std::uint8_t {
    Inland,
    DeepOcean,
    ShallowSea,
    Beach,
    RockyCoast,
    TidalFlat,
};

struct Config final {
    double seaLevelMeters{0.0};
    double beachMaxElevationMeters{8.0};
    double beachMaxSlopeDegrees{12.0};
    double tidalRangeMeters{1.5};
    double tidePeriodSeconds{44'714.0};
    double shallowWaterDepthMeters{20.0};
    double breakingDepthRatio{0.78};
    double waterDensityKgPerM3{1'025.0};
    std::uint64_t syntheticSeed{0x0CEA2026ULL};
    double equatorialTemperatureC{27.0};
    double polarTemperatureC{-1.0};
    double seasonalTemperatureAmplitudeC{3.0};
    double baseSalinityPsu{34.5};
    double maximumCurrentMetersPerSecond{1.6};
};

struct Forcing final {
    double simulationSeconds{0.0};
    double windSpeedMetersPerSecond{0.0};
    double windDirectionDegrees{0.0};
    double fetchMeters{0.0};
    double swellHeightMeters{0.0};
    double swellPeriodSeconds{12.0};
    double tidePhaseRadians{0.0};
    double seasonalPhaseRadians{0.0};
};

struct CoastSample final {
    globe::LatLon location{};
    double terrainElevationMeters{0.0};
    double seabedDepthMeters{0.0};
    double beachSlopeDegrees{0.0};
};

struct OceanState final {
    CoastalZone zone{CoastalZone::Inland};
    bool submerged{false};
    bool breaking{false};
    double gravityMetersPerSecondSquared{0.0};
    double tideMeters{0.0};
    double waterDepthMeters{0.0};
    double surfaceElevationMeters{0.0};
    double significantWaveHeightMeters{0.0};
    double dominantPeriodSeconds{0.0};
    double wavelengthMeters{0.0};
    double crestSpeedMetersPerSecond{0.0};
    double foamFactor{0.0};
    double surfaceVelocityXMetersPerSecond{0.0};
    double surfaceVelocityZMetersPerSecond{0.0};
    double currentEastMetersPerSecond{0.0};
    double currentNorthMetersPerSecond{0.0};
    double waterTemperatureC{0.0};
    double salinityPsu{0.0};
    double waterDensityKgPerM3{0.0};
    earth::GravitySample gravity{};
};

struct BuoyancyResult final {
    double accelerationMetersPerSecondSquared{0.0};
    globe::Vec3 acceleration{};
};

[[nodiscard]] bool valid_config(const Config& config) noexcept;
[[nodiscard]] bool valid_forcing(const Forcing& forcing) noexcept;
[[nodiscard]] bool valid_coast_sample(const CoastSample& sample) noexcept;

[[nodiscard]] OceanState sample(const Config& config,
                                const Forcing& forcing,
                                const CoastSample& coast) noexcept;

[[nodiscard]] BuoyancyResult buoyancy(const Config& config,
                                      const OceanState& state,
                                      double displacedVolumeM3,
                                      double bodyMassKg,
                                      double submergedFraction = 1.0) noexcept;

} // namespace indoru::ocean
