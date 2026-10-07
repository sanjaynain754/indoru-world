#include "indoru/globe_projection.hpp"

#include <algorithm>
#include <cmath>

namespace indoru::globe {
namespace {
constexpr double Pi = 3.14159265358979323846;
constexpr double HalfTurn = 180.0;
constexpr double FullTurn = 360.0;

double radians(double degrees) noexcept { return degrees * Pi / HalfTurn; }
double degrees(double value) noexcept { return value * HalfTurn / Pi; }
double clamp(double value, double low, double high) noexcept { return std::max(low, std::min(high, value)); }

double smoothstep(double value) noexcept {
    const double t = clamp(value, 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}
}

bool valid(LatLon coordinate) noexcept {
    return coordinate.latitudeDegrees >= -90.0 && coordinate.latitudeDegrees <= 90.0 &&
           std::isfinite(coordinate.latitudeDegrees) && std::isfinite(coordinate.longitudeDegrees);
}

LatLon normalize(LatLon coordinate) noexcept {
    coordinate.latitudeDegrees = clamp(coordinate.latitudeDegrees, -90.0, 90.0);
    coordinate.longitudeDegrees = std::fmod(coordinate.longitudeDegrees + HalfTurn, FullTurn);
    if (coordinate.longitudeDegrees < 0.0) coordinate.longitudeDegrees += FullTurn;
    coordinate.longitudeDegrees -= HalfTurn;
    return coordinate;
}

Vec3 to_unit_sphere(LatLon coordinate) noexcept {
    coordinate = normalize(coordinate);
    const double latitude = radians(coordinate.latitudeDegrees);
    const double longitude = radians(coordinate.longitudeDegrees);
    const double cosLatitude = std::cos(latitude);
    return {cosLatitude * std::cos(longitude), std::sin(latitude), cosLatitude * std::sin(longitude)};
}

LatLon from_unit_sphere(Vec3 position) noexcept {
    const double length = std::sqrt(position.x * position.x + position.y * position.y + position.z * position.z);
    if (length <= 1e-12 || !std::isfinite(length)) return {};
    position.x /= length;
    position.y /= length;
    position.z /= length;
    return normalize({degrees(std::asin(clamp(position.y, -1.0, 1.0))), degrees(std::atan2(position.z, position.x))});
}

double shortest_longitude_delta(double fromDegrees, double toDegrees) noexcept {
    double delta = std::fmod(toDegrees - fromDegrees + HalfTurn, FullTurn);
    if (delta < 0.0) delta += FullTurn;
    return delta - HalfTurn;
}

LatLon interpolate_shortest(LatLon from, LatLon to, double alpha) noexcept {
    from = normalize(from);
    to = normalize(to);
    const double t = smoothstep(alpha);
    return normalize({from.latitudeDegrees + (to.latitudeDegrees - from.latitudeDegrees) * t,
                      from.longitudeDegrees + shortest_longitude_delta(from.longitudeDegrees, to.longitudeDegrees) * t});
}

LatLon sample(PinAnimation& animation, double deltaSeconds) noexcept {
    animation.elapsedSeconds = std::max(0.0, animation.elapsedSeconds + std::max(0.0, deltaSeconds));
    const double duration = std::max(1e-6, animation.durationSeconds);
    return interpolate_shortest(animation.start, animation.target, animation.elapsedSeconds / duration);
}

} // namespace indoru::globe
