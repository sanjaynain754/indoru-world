#include "indoru/planet_terrain.hpp"

#include <cassert>
#include <cmath>

int main() {
    using namespace indoru;
    terrain::Config config{};
    assert(terrain::valid_config(config));

    const auto tile = terrain::tile_for({20.0, 72.0}, 4U);
    assert(terrain::valid_tile(tile));
    const auto tileBounds = terrain::bounds(tile);
    assert(tileBounds.southwest.latitudeDegrees <= 20.0);
    assert(tileBounds.northeast.latitudeDegrees >= 20.0);
    assert(tileBounds.southwest.longitudeDegrees <= 72.0);
    assert(tileBounds.northeast.longitudeDegrees >= 72.0);

    const auto first = terrain::sample(config, {20.0, 72.0});
    const auto repeat = terrain::sample(config, {20.0, 72.0});
    assert(first.elevationMeters == repeat.elevationMeters);
    assert(first.slopeDegrees >= 0.0);
    assert(first.moisture >= 0.0 && first.moisture <= 1.0);
    assert(std::isfinite(first.elevationMeters));

    const auto grid = terrain::generate_grid(config, tile, 8U);
    assert(grid.size() == 64U);
    assert(terrain::generate_grid(config, tile, 1U).empty());
    assert(terrain::generate_grid(config, {30U, 0U, 0U}, 8U).empty());
    return 0;
}
