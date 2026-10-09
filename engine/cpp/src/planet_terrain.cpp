#include "indoru/planet_terrain.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace indoru::terrain {
namespace {
constexpr double Pi = 3.14159265358979323846;
constexpr double DegreesToRadians = Pi / 180.0;
constexpr double EarthRadiusMeters = 6'378'137.0;
constexpr std::uint32_t MaximumLevel = 20U;

double clamp(double value, double low, double high) noexcept {
    return std::max(low, std::min(high, value));
}

double smooth(double value) noexcept {
    const double t = clamp(value, 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

double field(double latitude, double longitude, std::uint64_t seed) noexcept {
    const double phase = static_cast<double>(seed % 65'536U) * 0.00011;
    const double broad = std::sin(latitude * 1.6 + phase) * 0.55 + std::cos(longitude * 2.1 - phase) * 0.45;
    const double regional = std::sin(latitude * 4.8 - longitude * 2.7 + phase * 1.7) * 0.25 +
                            std::cos(latitude * 8.2 + longitude * 5.3) * 0.15;
    return broad + regional;
}

double raw_elevation(const Config& config, globe::LatLon location) noexcept {
    const double latitude = location.latitudeDegrees * DegreesToRadians;
    const double longitude = location.longitudeDegrees * DegreesToRadians;
    const double continental = field(latitude, longitude, config.seed);
    const double landBlend = smooth((continental + 0.35) / 0.8);
    const double ridge = std::abs(std::sin(latitude * 7.0 + longitude * 3.0)) *
                         std::abs(std::cos(latitude * 2.0 - longitude * 5.0));
    const double landHeight = config.seaLevelMeters + landBlend * config.maximumLandElevationMeters *
                              (0.35 + 0.65 * ridge);
    const double oceanDepth = config.seaLevelMeters - (1.0 - landBlend) * config.maximumOceanDepthMeters *
                              (0.45 + 0.55 * (1.0 - ridge));
    return landHeight * landBlend + oceanDepth * (1.0 - landBlend);
}
}

bool valid_config(const Config& config) noexcept {
    return std::isfinite(config.seaLevelMeters) && std::isfinite(config.maximumLandElevationMeters) &&
           config.maximumLandElevationMeters > 0.0 && std::isfinite(config.maximumOceanDepthMeters) &&
           config.maximumOceanDepthMeters > 0.0;
}

bool valid_tile(TileId tile) noexcept {
    if (tile.level > MaximumLevel) return false;
    const std::uint32_t count = 1U << tile.level;
    return tile.x < count && tile.y < count;
}

TileId tile_for(globe::LatLon location, std::uint32_t level) noexcept {
    if (level > MaximumLevel || !globe::valid(location)) return {};
    location = globe::normalize(location);
    const std::uint32_t count = 1U << level;
    const double xValue = (location.longitudeDegrees + 180.0) / 360.0 * static_cast<double>(count);
    const double yValue = (90.0 - location.latitudeDegrees) / 180.0 * static_cast<double>(count);
    const auto x = static_cast<std::uint32_t>(std::min(static_cast<double>(count - 1U), std::floor(xValue)));
    const auto y = static_cast<std::uint32_t>(std::min(static_cast<double>(count - 1U), std::floor(yValue)));
    return {level, x, y};
}

Bounds bounds(TileId tile) noexcept {
    if (!valid_tile(tile)) return {};
    const double count = static_cast<double>(1U << tile.level);
    const double west = -180.0 + 360.0 * static_cast<double>(tile.x) / count;
    const double east = -180.0 + 360.0 * static_cast<double>(tile.x + 1U) / count;
    const double north = 90.0 - 180.0 * static_cast<double>(tile.y) / count;
    const double south = 90.0 - 180.0 * static_cast<double>(tile.y + 1U) / count;
    return {{south, west}, {north, east}};
}

Sample sample(const Config& config, globe::LatLon location) noexcept {
    Sample result;
    if (!valid_config(config) || !globe::valid(location)) return result;
    location = globe::normalize(location);
    result.location = location;
    result.elevationMeters = raw_elevation(config, location);
    result.ocean = result.elevationMeters < config.seaLevelMeters;
    result.bathymetryDepthMeters = result.ocean ? config.seaLevelMeters - result.elevationMeters : 0.0;

    const double latitudeDelta = 0.02;
    const double longitudeDelta = 0.02 / std::max(0.1, std::cos(location.latitudeDegrees * DegreesToRadians));
    const auto north = raw_elevation(config, globe::normalize({location.latitudeDegrees + latitudeDelta, location.longitudeDegrees}));
    const auto east = raw_elevation(config, globe::normalize({location.latitudeDegrees, location.longitudeDegrees + longitudeDelta}));
    const double horizontalDistance = EarthRadiusMeters * latitudeDelta * DegreesToRadians;
    const double slope = std::hypot((north - result.elevationMeters) / horizontalDistance,
                                    (east - result.elevationMeters) / horizontalDistance);
    result.slopeDegrees = std::atan(slope) / DegreesToRadians;
    const double latitude = location.latitudeDegrees * DegreesToRadians;
    const double longitude = location.longitudeDegrees * DegreesToRadians;
    result.moisture = clamp(0.52 + 0.22 * std::cos(latitude) + 0.16 * std::sin(longitude * 3.0 + latitude * 2.0), 0.0, 1.0);
    return result;
}

std::vector<Sample> generate_grid(const Config& config, TileId tile, std::uint32_t resolution) noexcept {
    if (!valid_config(config) || !valid_tile(tile) || resolution < 2U || resolution > 256U) return {};
    const auto tileBounds = bounds(tile);
    std::vector<Sample> grid;
    grid.reserve(static_cast<std::size_t>(resolution) * static_cast<std::size_t>(resolution));
    for (std::uint32_t row = 0; row < resolution; ++row) {
        const double v = static_cast<double>(row) / static_cast<double>(resolution - 1U);
        const double latitude = tileBounds.northeast.latitudeDegrees +
                                (tileBounds.southwest.latitudeDegrees - tileBounds.northeast.latitudeDegrees) * v;
        for (std::uint32_t column = 0; column < resolution; ++column) {
            const double u = static_cast<double>(column) / static_cast<double>(resolution - 1U);
            const double longitude = tileBounds.southwest.longitudeDegrees +
                                     (tileBounds.northeast.longitudeDegrees - tileBounds.southwest.longitudeDegrees) * u;
            grid.push_back(sample(config, {latitude, longitude}));
        }
    }
    return grid;
}

} // namespace indoru::terrain
