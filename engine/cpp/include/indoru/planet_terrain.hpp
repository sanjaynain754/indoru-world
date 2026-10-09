#pragma once

#include "indoru/globe_projection.hpp"

#include <cstdint>
#include <vector>

namespace indoru::terrain {

struct Config final {
    std::uint64_t seed{0x1D0A2026ULL};
    double seaLevelMeters{0.0};
    double maximumLandElevationMeters{4'000.0};
    double maximumOceanDepthMeters{6'000.0};
};

struct TileId final {
    std::uint32_t level{0};
    std::uint32_t x{0};
    std::uint32_t y{0};
};

struct Bounds final {
    globe::LatLon southwest{};
    globe::LatLon northeast{};
};

struct Sample final {
    globe::LatLon location{};
    double elevationMeters{0.0};
    double bathymetryDepthMeters{0.0};
    double slopeDegrees{0.0};
    double moisture{0.0};
    bool ocean{false};
};

[[nodiscard]] bool valid_config(const Config& config) noexcept;
[[nodiscard]] bool valid_tile(TileId tile) noexcept;
[[nodiscard]] TileId tile_for(globe::LatLon location, std::uint32_t level) noexcept;
[[nodiscard]] Bounds bounds(TileId tile) noexcept;
[[nodiscard]] Sample sample(const Config& config, globe::LatLon location) noexcept;
[[nodiscard]] std::vector<Sample> generate_grid(const Config& config,
                                                 TileId tile,
                                                 std::uint32_t resolution) noexcept;

} // namespace indoru::terrain
