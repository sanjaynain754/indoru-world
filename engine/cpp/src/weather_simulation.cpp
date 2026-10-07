#include "indoru/weather_simulation.hpp"

#include <algorithm>
#include <cmath>

namespace indoru {
namespace {

std::uint64_t Mix(std::uint64_t value) {
    value ^= value >> 30U;
    value *= 0xbf58476d1ce4e5b9ULL;
    value ^= value >> 27U;
    value *= 0x94d049bb133111ebULL;
    return value ^ (value >> 31U);
}

float Unit(std::uint64_t seed) {
    return static_cast<float>((Mix(seed) >> 11U) & 0xFFFFFFU) / 16'777'215.0f;
}

bool Contains(const std::vector<WeatherCondition>& hazards, WeatherCondition condition) {
    for (const auto hazard : hazards) if (hazard == condition) return true;
    return false;
}

} // namespace

bool WeatherSimulation::ValidateProfile(const WeatherProfile& profile) {
    if (profile.countryId.empty() || profile.climate.empty()) return false;
    if (profile.minRoadGrip < 0.0f || profile.maxRoadGrip > 1.0f || profile.minRoadGrip > profile.maxRoadGrip) return false;
    if (profile.minVisibilityMeters <= 0.0f || profile.minVisibilityMeters > profile.maxVisibilityMeters) return false;
    if (profile.minNpcActivity < 0.0f || profile.maxNpcActivity > 1.0f || profile.minNpcActivity > profile.maxNpcActivity) return false;
    return true;
}

bool WeatherSimulation::IsHazardActive(const WeatherProfile& profile, WeatherCondition condition) {
    return ValidateProfile(profile) && Contains(profile.hazards, condition);
}

WeatherState WeatherSimulation::Sample(const WeatherProfile& profile,
                                       std::uint32_t simulationDay,
                                       std::uint16_t minuteOfDay) {
    WeatherState state;
    if (!ValidateProfile(profile) || minuteOfDay >= 1'440U) return state;

    const auto timeBucket = static_cast<std::uint64_t>(minuteOfDay / 15U);
    const auto sampleSeed = profile.seed ^ (static_cast<std::uint64_t>(simulationDay) * 0x9e3779b97f4a7c15ULL) ^ timeBucket;
    const float roll = Unit(sampleSeed);
    const float dailyWave = std::sin(static_cast<float>(minuteOfDay) * 0.0043633231f);
    state.intensity = std::clamp(0.15f + roll * 0.7f + dailyWave * 0.1f, 0.0f, 1.0f);

    if (roll > 0.93f && Contains(profile.hazards, WeatherCondition::Blizzard)) state.condition = WeatherCondition::Blizzard;
    else if (roll > 0.88f && Contains(profile.hazards, WeatherCondition::DustStorm)) state.condition = WeatherCondition::DustStorm;
    else if (roll > 0.83f && Contains(profile.hazards, WeatherCondition::Thunderstorm)) state.condition = WeatherCondition::Thunderstorm;
    else if (roll > 0.78f && Contains(profile.hazards, WeatherCondition::FlashFlood)) state.condition = WeatherCondition::FlashFlood;
    else if (roll > 0.74f && Contains(profile.hazards, WeatherCondition::Heatwave)) state.condition = WeatherCondition::Heatwave;
    else if (roll > 0.70f && Contains(profile.hazards, WeatherCondition::ColdSnap)) state.condition = WeatherCondition::ColdSnap;
    else if (roll > 0.48f) state.condition = WeatherCondition::Cloudy;
    else if (roll > 0.30f) state.condition = WeatherCondition::Rain;

    const float hazardPenalty = state.condition == WeatherCondition::Clear ? 0.0f : state.intensity * 0.45f;
    state.roadGrip = std::clamp(profile.maxRoadGrip - hazardPenalty, profile.minRoadGrip, profile.maxRoadGrip);
    state.visibilityMeters = std::clamp(profile.maxVisibilityMeters * (1.0f - state.intensity * 0.85f), profile.minVisibilityMeters, profile.maxVisibilityMeters);
    state.npcActivity = std::clamp(profile.maxNpcActivity - state.intensity * 0.5f, profile.minNpcActivity, profile.maxNpcActivity);
    state.temperatureC = 20.0f + dailyWave * 9.0f;
    if (state.condition == WeatherCondition::Heatwave) state.temperatureC += 14.0f * state.intensity;
    if (state.condition == WeatherCondition::ColdSnap || state.condition == WeatherCondition::Blizzard) state.temperatureC -= 18.0f * state.intensity;
    state.railServiceDelayed = state.condition == WeatherCondition::Blizzard || state.condition == WeatherCondition::FlashFlood || state.visibilityMeters < 500.0f;
    state.riverNavigationRestricted = state.condition == WeatherCondition::FlashFlood || state.condition == WeatherCondition::Blizzard;
    state.emergencyResponseActive = state.intensity > 0.65f || state.condition == WeatherCondition::FlashFlood;
    return state;
}

} // namespace indoru
