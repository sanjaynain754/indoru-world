#include "indoru/earth_gravity.hpp"

#include <algorithm>
#include <cmath>

namespace indoru::earth {
namespace {
constexpr double Pi = 3.14159265358979323846;
constexpr double SemiMajorAxisMeters = 6'378'137.0;
constexpr double Flattening = 1.0 / 298.257223563;
constexpr double EccentricitySquared = Flattening * (2.0 - Flattening);
constexpr double EquatorialGravity = 9.7803253359;
constexpr double SomiglianaK = 0.00193185265241;

double radians(double degrees) noexcept { return degrees * Pi / 180.0; }
}

bool valid_inputs(double latitudeDegrees, double altitudeMeters) noexcept {
    return std::isfinite(latitudeDegrees) && std::isfinite(altitudeMeters) &&
           latitudeDegrees >= -90.0 && latitudeDegrees <= 90.0 &&
           altitudeMeters >= -1'000.0 && altitudeMeters <= 100'000.0;
}

double normal_gravity(double latitudeDegrees, double altitudeMeters) noexcept {
    if (!valid_inputs(latitudeDegrees, altitudeMeters)) return 0.0;
    const double latitude = radians(latitudeDegrees);
    const double sine = std::sin(latitude);
    const double sineSquared = sine * sine;
    const double surfaceGravity = EquatorialGravity * (1.0 + SomiglianaK * sineSquared) /
                                  std::sqrt(1.0 - EccentricitySquared * sineSquared);
    const double heightRatio = altitudeMeters / SemiMajorAxisMeters;
    return surfaceGravity * (1.0 - 2.0 * heightRatio + 3.0 * heightRatio * heightRatio);
}

GravitySample sample(double latitudeDegrees, double longitudeDegrees, double altitudeMeters) noexcept {
    if (!valid_inputs(latitudeDegrees, altitudeMeters) || !std::isfinite(longitudeDegrees)) return {};
    const double latitude = radians(latitudeDegrees);
    const double longitude = radians(longitudeDegrees);
    const double cosLatitude = std::cos(latitude);
    const double downX = cosLatitude * std::cos(longitude);
    const double downY = std::sin(latitude);
    const double downZ = cosLatitude * std::sin(longitude);
    const double length = std::sqrt(downX * downX + downY * downY + downZ * downZ);
    const double acceleration = normal_gravity(latitudeDegrees, altitudeMeters);
    if (length <= 1e-12) return {};
    return {acceleration, downX / length, downY / length, downZ / length};
}

} // namespace indoru::earth
