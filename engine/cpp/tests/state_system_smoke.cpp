#include "indoru/state_system.hpp"
#include <cassert>
#include <limits>

using namespace indoru;

int main() {
    std::vector<StateRecord> states = {
        {"indoru-world-001", "", "", "Indoru", StateLevel::World, 0, true},
        {"region-avr", "indoru-world-001", "", "Avarra Crescent", StateLevel::Region, 0, true},
        {"state-country-001", "region-avr", "country-001", "Avenra", StateLevel::Country, 1000, true},
        {"state-avenra-capital", "state-country-001", "country-001", "Avenra Central", StateLevel::City, 800, true},
        {"state-avenra-village-1", "state-country-001", "country-001", "Avenra Village", StateLevel::Settlement, 200, true},
    };
    assert(StateSystem::ValidateHierarchy("indoru-world-001", states));
    assert(!StateSystem::ValidateHierarchy("indoru-world-001", {states[0], states[2]}));

    auto& country = states[2];
    assert(StateSystem::ApplyPopulationDelta(country, 500));
    assert(country.population == 1500);
    assert(StateSystem::ApplyPopulationDelta(country, -500));
    assert(country.population == 1000);
    assert(!StateSystem::ApplyPopulationDelta(country, -1001));
    country.population = std::numeric_limits<std::uint64_t>::max();
    assert(!StateSystem::ApplyPopulationDelta(country, 1));
    return 0;
}
