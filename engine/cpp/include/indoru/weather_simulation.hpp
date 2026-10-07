#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace indoru {

enum class WeatherCondition : std::uint8_t {
    Clear,
    Cloudy,
    Rain,
    DustStorm,
    Blizzard,
    Thunderstorm,
    Heatwave,
    ColdSnap,
    FlashFlood
};

struct WeatherProfile {
    std::string countryId;
    std::string climate;
    std::uint64_t seed{0};
    float minRoadGrip{0.5f};
    float maxRoadGrip{1.0f};
    float minVisibilityMeters{50.0f};
    float maxVisibilityMeters{20'000.0f};
    float minNpcActivity{0.3f};
    float maxNpcActivity{1.0f};
    std::vector<WeatherCondition> hazards;
};

struct WeatherState {
    WeatherCondition condition{WeatherCondition::Clear};
    float intensity{0.0f};
    float temperatureC{20.0f};
    float roadGrip{1.0f};
    float visibilityMeters{20'000.0f};
    float npcActivity{1.0f};
    bool railServiceDelayed{false};
    bool riverNavigationRestricted{false};
    bool emergencyResponseActive{false};
};

class WeatherSimulation {
public:
    static bool ValidateProfile(const WeatherProfile& profile);

    static WeatherState Sample(const WeatherProfile& profile,
                               std::uint32_t simulationDay,
                               std::uint16_t minuteOfDay);

    static bool IsHazardActive(const WeatherProfile& profile,
                               WeatherCondition condition);
};

} // namespace indoru
