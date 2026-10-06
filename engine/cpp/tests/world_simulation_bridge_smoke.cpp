#include "indoru/world_simulation_bridge.hpp"
#include <cassert>

using namespace indoru;

int main() {
    CountrySimulationInput input;
    input.countryId = "country-001";
    input.population = 100'000;
    input.locationIds = {"avenra-capital", "avenra-coast"};
    input.transportLinks = {
        {"road-001", "avenra-capital", TransportMode::Road, 500.0f, 4, true, true},
        {"rail-001", "avenra-capital", TransportMode::Rail, 800.0f, 2, true, true},
        {"river-001", "avenra-coast", TransportMode::River, 1'200.0f, 1, false, true},
        {"invalid", "avenra-capital", TransportMode::Road, 0.0f, 2, true, false},
    };

    assert(WorldSimulationBridge::ValidateTransportLink(input.transportLinks[0]));
    assert(!WorldSimulationBridge::ValidateTransportLink(input.transportLinks[3]));

    const auto plan = WorldSimulationBridge::BuildPlan(input, 100, 8);
    assert(plan.countryId == "country-001");
    assert(plan.locations.size() == 2);
    assert(plan.validTransportLinks == 3);
    assert(plan.locations[0].roadLinks == 1);
    assert(plan.locations[0].railLinks == 1);
    assert(plan.locations[1].riverLinks == 1);
    assert(plan.locations[0].city.plots > 0);
    assert(plan.locations[0].residential.targetResidents > 0);
    return 0;
}
