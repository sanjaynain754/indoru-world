#include "indoru/weather_simulation.hpp"
#include <cassert>

using namespace indoru;

int main() {
    WeatherProfile profile{
        "country-021", "arid-canyon", 0xA57EULL,
        0.55f, 1.0f, 80.0f, 20'000.0f,
        0.35f, 1.0f,
        {WeatherCondition::DustStorm, WeatherCondition::FlashFlood, WeatherCondition::Heatwave}
    };
    assert(WeatherSimulation::ValidateProfile(profile));
    assert(WeatherSimulation::IsHazardActive(profile, WeatherCondition::DustStorm));
    assert(!WeatherSimulation::IsHazardActive(profile, WeatherCondition::Blizzard));

    const auto first = WeatherSimulation::Sample(profile, 4, 720);
    const auto repeat = WeatherSimulation::Sample(profile, 4, 720);
    assert(first.condition == repeat.condition);
    assert(first.roadGrip == repeat.roadGrip);
    assert(first.visibilityMeters == repeat.visibilityMeters);
    assert(first.roadGrip >= profile.minRoadGrip && first.roadGrip <= profile.maxRoadGrip);
    assert(first.visibilityMeters >= profile.minVisibilityMeters);
    assert(first.npcActivity >= profile.minNpcActivity);

    assert(WeatherSimulation::Sample(profile, 1, 1440).condition == WeatherCondition::Clear);
    WeatherProfile invalid = profile;
    invalid.maxRoadGrip = 1.2f;
    assert(!WeatherSimulation::ValidateProfile(invalid));
    return 0;
}
