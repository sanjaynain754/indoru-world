#include "indoru/planet_atmosphere.hpp"

#include <cassert>
#include <cmath>

int main() {
    using namespace indoru;
    atmosphere::Config config{};
    atmosphere::Forcing forcing{};
    forcing.simulationSeconds = 7'200.0;
    forcing.orbitalPhaseRadians = 0.7;
    forcing.terrainElevationMeters = 800.0;
    forcing.weatherIntensity = 0.65;
    forcing.overOcean = true;

    assert(atmosphere::valid_config(config));
    assert(atmosphere::valid_forcing(forcing));
    const auto state = atmosphere::sample(config, forcing, {24.0, 72.0});
    assert(state.pressurePa > 0.0);
    assert(state.airDensityKgPerM3 > 0.0);
    assert(state.relativeHumidity >= 0.0 && state.relativeHumidity <= 1.0);
    assert(state.cloudFactor >= 0.0 && state.cloudFactor <= 1.0);
    assert(state.windSpeedMetersPerSecond <= config.maximumWindMetersPerSecond);
    assert(std::isfinite(state.windDirectionDegrees));
    assert(atmosphere::sample(config, forcing, {91.0, 0.0}).pressurePa == 0.0);
    return 0;
}
