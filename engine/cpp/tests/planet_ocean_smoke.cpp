#include "indoru/planet_ocean.hpp"

#include <cassert>
#include <cmath>

int main() {
    using namespace indoru;
    ocean::Config config{};
    ocean::Forcing forcing{};
    forcing.windSpeedMetersPerSecond = 12.0;
    forcing.fetchMeters = 50'000.0;
    forcing.swellHeightMeters = 1.0;
    forcing.swellPeriodSeconds = 10.0;

    assert(ocean::valid_config(config));
    assert(ocean::valid_forcing(forcing));

    const ocean::CoastSample deepWater{{0.0, 0.0}, -10.0, 100.0, 0.0};
    const auto deep = ocean::sample(config, forcing, deepWater);
    assert(deep.submerged);
    assert(deep.zone == ocean::CoastalZone::DeepOcean);
    assert(deep.significantWaveHeightMeters > 0.0);
    assert(deep.dominantPeriodSeconds >= 2.0);
    assert(deep.waterTemperatureC > config.polarTemperatureC);
    assert(deep.salinityPsu >= 28.0 && deep.salinityPsu <= 40.0);
    assert(deep.waterDensityKgPerM3 > 0.0);
    assert(std::isfinite(deep.currentEastMetersPerSecond));
    assert(std::isfinite(deep.surfaceElevationMeters));

    const ocean::CoastSample beach{{20.0, 30.0}, 2.0, 0.0, 5.0};
    const auto beachState = ocean::sample(config, ocean::Forcing{}, beach);
    assert(!beachState.submerged);
    assert(beachState.zone == ocean::CoastalZone::Beach);

    const ocean::CoastSample rockyCoast{{20.0, 30.0}, 20.0, 0.0, 30.0};
    assert(ocean::sample(config, ocean::Forcing{}, rockyCoast).zone == ocean::CoastalZone::RockyCoast);

    const ocean::CoastSample shallowWater{{-45.0, 120.0}, -1.0, 3.0, 0.0};
    const auto shallow = ocean::sample(config, forcing, shallowWater);
    assert(shallow.zone == ocean::CoastalZone::ShallowSea);
    assert(shallow.breaking);
    assert(shallow.foamFactor >= 0.0 && shallow.foamFactor <= 1.0);

    const auto floatResult = ocean::buoyancy(config, deep, 2.0, 1'000.0, 1.0);
    assert(floatResult.accelerationMetersPerSecondSquared > 0.0);
    assert(std::isfinite(floatResult.acceleration.x));
    assert(ocean::buoyancy(config, beachState, 2.0, 1'000.0).accelerationMetersPerSecondSquared == 0.0);
    return 0;
}
