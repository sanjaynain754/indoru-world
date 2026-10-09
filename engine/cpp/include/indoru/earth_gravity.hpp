#pragma once

namespace indoru::earth {

struct GravitySample final {
    double accelerationMetersPerSecondSquared{0.0};
    double downX{0.0};
    double downY{0.0};
    double downZ{0.0};
};

// WGS84 normal gravity on the reference ellipsoid with a bounded altitude correction.
// Latitude is geodetic latitude in degrees; north is positive. Altitude is metres.
[[nodiscard]] bool valid_inputs(double latitudeDegrees, double altitudeMeters) noexcept;
[[nodiscard]] double normal_gravity(double latitudeDegrees, double altitudeMeters = 0.0) noexcept;
[[nodiscard]] GravitySample sample(double latitudeDegrees, double longitudeDegrees, double altitudeMeters = 0.0) noexcept;

} // namespace indoru::earth
