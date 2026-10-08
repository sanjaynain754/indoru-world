#include "indoru/planet_day_night.hpp"
#include <cassert>

int main() {
    using namespace indoru;
    planet::Clock clock{};
    assert(planet::valid_clock(clock));
    assert(!planet::valid_clock(planet::Clock{0.0, 0.0, 1.0, 23.0}));

    const auto noon = planet::sample_sun({0.0, 0.0}, clock);
    assert(noon.daylight);
    assert(noon.solarElevationDegrees > 80.0);
    assert(planet::local_solar_hour({0.0, 0.0}, clock) > 11.9);

    clock.simulationSeconds = 43'200.0;
    const auto midnight = planet::sample_sun({0.0, 0.0}, clock);
    assert(!midnight.daylight);
    assert(midnight.solarElevationDegrees < -80.0);

    clock.simulationSeconds = 0.0;
    const auto northPole = planet::sample_sun({90.0, 0.0}, clock);
    assert(northPole.solarElevationDegrees > -1.0);
    clock.simulationSeconds = clock.orbitalPeriodSeconds / 2.0;
    const auto oppositeSeason = planet::sample_sun({90.0, 0.0}, clock);
    assert(oppositeSeason.solarElevationDegrees < northPole.solarElevationDegrees);

    clock.simulationSeconds = 43'200.0;
    const auto twilight = planet::sample_sun({66.0, 0.0}, clock);
    assert(twilight.daylightFactor >= 0.0 && twilight.daylightFactor <= 1.0);
    return 0;
}
