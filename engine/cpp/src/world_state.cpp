#include "indoru/world_state.hpp"

#include <mutex>
#include <utility>

namespace indoru::world {

WorldState::WorldState(std::string world_id)
    : world_id_(std::move(world_id)) {}

void WorldState::advance_tick() {
    std::unique_lock lock(mutex_);
    ++tick_;
}

Tick WorldState::current_tick() const noexcept {
    std::shared_lock lock(mutex_);
    return tick_;
}

bool WorldState::register_country(CountryState country) {
    if (country.id.empty() || country.name.empty() || country.flag_ref.empty()) {
        return false;
    }
    std::unique_lock lock(mutex_);
    return countries_.emplace(country.id, std::move(country)).second;
}

bool WorldState::register_settlement(SettlementState settlement) {
    if (settlement.id.empty() || settlement.name.empty() || settlement.country_id.empty()) {
        return false;
    }
    std::unique_lock lock(mutex_);
    if (!countries_.contains(settlement.country_id)) {
        return false;
    }
    return settlements_.emplace(settlement.id, std::move(settlement)).second;
}

bool WorldState::set_weather(WeatherState weather) {
    if (weather.visibility < 0.0F || weather.visibility > 1.0F) {
        return false;
    }
    std::unique_lock lock(mutex_);
    weather_ = weather;
    return true;
}

bool WorldState::set_active_country(std::string country_id) {
    std::unique_lock lock(mutex_);
    if (!countries_.contains(country_id)) {
        return false;
    }
    active_country_id_ = std::move(country_id);
    return true;
}

std::optional<CountryState> WorldState::country(std::string_view id) const {
    std::shared_lock lock(mutex_);
    const auto it = countries_.find(std::string(id));
    if (it == countries_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::optional<SettlementState> WorldState::settlement(std::string_view id) const {
    std::shared_lock lock(mutex_);
    const auto it = settlements_.find(std::string(id));
    if (it == settlements_.end()) {
        return std::nullopt;
    }
    return it->second;
}

WorldSnapshot WorldState::snapshot() const {
    std::shared_lock lock(mutex_);
    WorldSnapshot result;
    result.tick = tick_;
    result.world_id = world_id_;
    result.active_country_id = active_country_id_;
    result.weather = weather_;
    result.settlements.reserve(settlements_.size());
    for (const auto& [_, settlement] : settlements_) {
        result.settlements.push_back(settlement);
    }
    return result;
}

} // namespace indoru::world
