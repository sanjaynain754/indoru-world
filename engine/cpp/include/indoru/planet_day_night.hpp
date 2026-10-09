#pragma once

#include "indoru/globe_projection.hpp"

namespace indoru::planet {

struct Clock final {
    double simulationSeconds{0.0};
    double solarDaySeconds{86'400.0};
    double orbitalPeriodSeconds{31'558'149.7635456};
    double axialTiltDegrees{23.43928};
};

struct SunState final {
    double declinationDegrees{0.0};
    double subsolarLongitudeDegrees{0.0};
    double solarElevationDegrees{-90.0};
    double solarAzimuthDegrees{0.0};
    double daylightFactor{0.0};
    bool daylight{false};
    bool civilTwilight{false};
};

[[nodiscard]] bool valid_clock(const Clock& clock) noexcept;
[[nodiscard]] SunState sample_sun(globe::LatLon location, const Clock& clock) noexcept;
[[nodiscard]] double local_solar_hour(globe::LatLon location, const Clock& clock) noexcept;

} // namespace indoru::planet
