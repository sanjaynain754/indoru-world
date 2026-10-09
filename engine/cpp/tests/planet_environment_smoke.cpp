#include "indoru/planet_environment.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

int main() {
    using namespace indoru;

    environment::Config config{};
    assert(environment::valid_config(config));
    assert(!environment::valid_config(environment::Config{planet::Clock{0.0, 0.0, 1.0, 23.0}}));

    auto badSwell = config;
    badSwell.swellPeriodSeconds = 40.0;
    assert(!environment::valid_config(badSwell));
    auto badOffset = config;
    badOffset.seabedDepthOffsetMeters = -1.0;
    assert(!environment::valid_config(badOffset));

    environment::Forcing forcing{};
    assert(environment::valid_forcing(forcing));
    auto badForcing = forcing;
    badForcing.weatherIntensity = 2.0;
    assert(!environment::valid_forcing(badForcing));

    const auto equator = environment::sample(config, forcing, {0.0, 0.0});
    assert(equator.valid);
    assert(equator.gravity.accelerationMetersPerSecondSquared > 9.7);
    assert(equator.gravity.accelerationMetersPerSecondSquared < 9.9);
    assert(equator.air.pressurePa > 0.0);
    assert(equator.air.airDensityKgPerM3 > 0.0);
    assert(equator.sun.daylight);
    assert(equator.sky.dayNightBlend > 0.9);
    assert(equator.water.gravityMetersPerSecondSquared > 0.0);

    // Single time model: the sky consumes planet::sample_sun without altering it.
    assert(std::abs(equator.sky.sunElevationDegrees - equator.sun.solarElevationDegrees) < 1e-12);
    assert(std::abs(equator.sky.sunAzimuthDegrees - equator.sun.solarAzimuthDegrees) < 1e-12);

    // Gravity is sampled at the terrain surface through the documented altitude clamp.
    const auto ground = terrain::sample(config.terrain, {0.0, 0.0});
    const double clampedAltitude = std::max(-1'000.0, std::min(100'000.0, ground.elevationMeters));
    const auto directGravity = earth::sample(0.0, 0.0, clampedAltitude);
    assert(std::abs(equator.gravity.accelerationMetersPerSecondSquared -
                    directGravity.accelerationMetersPerSecondSquared) < 1e-12);

    const auto pole = environment::sample(config, forcing, {89.0, 0.0});
    assert(pole.valid);
    assert(pole.gravity.accelerationMetersPerSecondSquared > equator.gravity.accelerationMetersPerSecondSquared);

    bool foundOcean = false;
    bool foundLand = false;
    for (int latitude = -80; latitude <= 80 && !(foundOcean && foundLand); latitude += 4) {
        for (int longitude = -180; longitude <= 180 && !(foundOcean && foundLand); longitude += 5) {
            const globe::LatLon location{static_cast<double>(latitude), static_cast<double>(longitude)};
            const auto tile = environment::sample(config, forcing, location);
            assert(tile.valid);
            if (tile.ground.ocean && !foundOcean && tile.ground.bathymetryDepthMeters > 20.0) {
                foundOcean = true;
                assert(tile.water.submerged);
                assert(tile.water.waterDepthMeters > 0.0);
                assert(tile.water.zone == ocean::CoastalZone::DeepOcean ||
                       tile.water.zone == ocean::CoastalZone::ShallowSea);
                const double expectedDepth = std::max(0.0, tile.water.tideMeters - tile.ground.elevationMeters);
                assert(std::abs(tile.water.waterDepthMeters - expectedDepth) < 1e-9);
            } else if (!tile.ground.ocean && !foundLand) {
                foundLand = true;
                assert(!tile.water.submerged);
                assert(tile.water.zone != ocean::CoastalZone::DeepOcean);
                assert(tile.water.zone != ocean::CoastalZone::ShallowSea);
            }
        }
    }
    assert(foundOcean);
    assert(foundLand);

    const auto repeat = environment::sample(config, forcing, {0.0, 0.0});
    assert(repeat.valid);
    assert(repeat.air.temperatureC == equator.air.temperatureC);
    assert(repeat.sky.sunIntensity == equator.sky.sunIntensity);
    assert(repeat.water.waterDepthMeters == equator.water.waterDepthMeters);

    const auto rejected = environment::sample(config, forcing, {91.0, 0.0});
    assert(!rejected.valid);
    assert(rejected.gravity.accelerationMetersPerSecondSquared == 0.0);
    assert(rejected.sky.sunIntensity == 0.0);
    assert(!environment::sample(config, badForcing, {0.0, 0.0}).valid);
    assert(!environment::sample(badSwell, forcing, {0.0, 0.0}).valid);
    return 0;
}
