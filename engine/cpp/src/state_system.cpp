#include "indoru/state_system.hpp"

#include <algorithm>
#include <limits>
#include <unordered_map>
#include <unordered_set>

namespace indoru {

bool StateSystem::ValidateRecord(const StateRecord& state) {
    if (state.stateId.empty() || state.name.empty()) return false;
    if (state.level == StateLevel::World) {
        return state.parentStateId.empty() && state.countryId.empty();
    }
    if (state.parentStateId.empty()) return false;
    if (state.level == StateLevel::Country && state.stateId != "state-" + state.countryId) return false;
    if (state.level >= StateLevel::City && state.countryId.empty()) return false;
    return true;
}

bool StateSystem::ValidateHierarchy(const std::string& worldId,
                                    const std::vector<StateRecord>& states) {
    if (worldId.empty() || states.empty()) return false;
    std::unordered_map<std::string, const StateRecord*> byId;
    byId.reserve(states.size());
    for (const auto& state : states) {
        if (!ValidateRecord(state) || !byId.emplace(state.stateId, &state).second) return false;
    }
    std::size_t worldCount = 0;
    for (const auto& state : states) {
        if (state.level == StateLevel::World) {
            ++worldCount;
            if (state.stateId != worldId) return false;
            continue;
        }
        const auto parent = byId.find(state.parentStateId);
        if (parent == byId.end()) return false;
        if (static_cast<unsigned>(parent->second->level) >= static_cast<unsigned>(state.level)) return false;
        if (!state.countryId.empty() && parent->second->level >= StateLevel::Country && parent->second->countryId != state.countryId) return false;
    }
    if (worldCount != 1) return false;

    for (const auto& state : states) {
        std::unordered_set<std::string> seen;
        const StateRecord* current = &state;
        while (current->level != StateLevel::World) {
            if (!seen.emplace(current->stateId).second) return false;
            const auto parent = byId.find(current->parentStateId);
            if (parent == byId.end()) return false;
            current = parent->second;
        }
    }
    return true;
}

bool StateSystem::ApplyPopulationDelta(StateRecord& state, std::int64_t delta) {
    if (!ValidateRecord(state)) return false;
    if (delta >= 0) {
        const auto increase = static_cast<std::uint64_t>(delta);
        if (state.population > std::numeric_limits<std::uint64_t>::max() - increase) return false;
        state.population += increase;
        return true;
    }
    const auto decrease = static_cast<std::uint64_t>(-(delta + 1)) + 1U;
    if (decrease > state.population) return false;
    state.population -= decrease;
    return true;
}

} // namespace indoru
