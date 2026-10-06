#include "indoru/residential_simulation.hpp"
#include <cassert>

using namespace indoru;

int main() {
    auto night = ResidentialSimulation::CalculateNightDistribution(1'000'000, 0.90);
    assert(night.residentialPopulation == 900'000);
    assert(night.nonResidentialPopulation == 100'000);

    ResidentialUnit unit{"unit-001", "building-001", "navaar-capital", "country-001", 11, {}};
    assert(ResidentialSimulation::CanFitHousehold(unit, 0, 11));
    assert(!ResidentialSimulation::CanFitHousehold(unit, 0, 12));

    Household family;
    family.householdId = "household-001";
    family.residentialUnitId = "unit-001";
    family.homeAssigned = true;
    family.members.resize(11);
    assert(ResidentialSimulation::ValidateHousehold(family));

    DailySchedule nightShift{"npc-001", "household-001",
                             HomeOccupancyState::NightShift, 22, 6, "hospital-001"};
    assert(ResidentialSimulation::StateAtHour(nightShift, 23) ==
           HomeOccupancyState::NightShift);
    assert(ResidentialSimulation::StateAtHour(nightShift, 12) ==
           HomeOccupancyState::Home);

    return 0;
}
