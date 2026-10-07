#pragma once

#include <cstdint>

namespace indoru::globe {

struct LatLon final {
    double latitudeDegrees{0.0};
    double longitudeDegrees{0.0};
};

struct Vec3 final {
    double x{0.0};
    double y{0.0};
    double z{0.0};
};

struct PinAnimation final {
    LatLon start{};
    LatLon target{};
    double durationSeconds{0.8};
    double elapsedSeconds{0.0};
};

[[nodiscard]] bool valid(LatLon coordinate) noexcept;
[[nodiscard]] LatLon normalize(LatLon coordinate) noexcept;
[[nodiscard]] Vec3 to_unit_sphere(LatLon coordinate) noexcept;
[[nodiscard]] LatLon from_unit_sphere(Vec3 position) noexcept;

// Returns the shortest wrapped longitude delta, avoiding dateline spins.
[[nodiscard]] double shortest_longitude_delta(double fromDegrees, double toDegrees) noexcept;

[[nodiscard]] LatLon interpolate_shortest(LatLon from, LatLon to, double alpha) noexcept;
[[nodiscard]] LatLon sample(PinAnimation& animation, double deltaSeconds) noexcept;

} // namespace indoru::globe
