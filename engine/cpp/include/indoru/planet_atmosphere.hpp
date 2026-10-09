#pragma once

#include "indoru/globe_projection.hpp"

#include <cstdint>

namespace indoru::atmosphere {

struct Config final {
    std::uint64_t seed{0x1D0A2026ULL};
    double seaLevelTemperatureC{18.0};
    double poleTemperatureDropC{32.0};
    double seasonalTemperatureAmplitudeC{8.0};
    double seaLevelPressurePa{101'325.0};
    double scaleHeightMeters{8'400.0};
    double maximumWindMetersPerSecond{35.0};
};

struct Forcing final {
    double simulationSeconds{0.0};
    double orbitalPhaseRadians{0.0};
    double terrainElevationMeters{0.0};
    double weatherIntensity{0.0};
    bool overOcean{false};
};

struct State final {
    double temperatureC{0.0};
    double pressurePa{0.0};
    double relativeHumidity{0.0};
    double cloudFactor{0.0};
    double precipitationMillimetersPerHour{0.0};
    double airDensityKgPerM3{0.0};
    double windEastMetersPerSecond{0.0};
    double windNorthMetersPerSecond{0.0};
    double windUpMetersPerSecond{0.0};
    double windSpeedMetersPerSecond{0.0};
    double windDirectionDegrees{0.0};
};

[[nodiscard]] bool valid_config(const Config& config) noexcept;
[[nodiscard]] bool valid_forcing(const Forcing& forcing) noexcept;
[[nodiscard]] State sample(const Config& config,
                           const Forcing& forcing,
                           globe::LatLon location) noexcept;

} // namespace indoru::atmosphere
