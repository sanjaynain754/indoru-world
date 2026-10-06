#include "indoru/residential_building_placement.hpp"
#include <cassert>

using namespace indoru;

int main() {
    assert(ResidentialBuildingPlanner::EstimateBuildings(100, 8) == 13);

    BuildingFootprint b{
        "building-001", "country-001", "location-001",
        BuildingType::ApartmentMidRise, 6, 24, 12, true, true
    };
    assert(ResidentialBuildingPlanner::ValidateFootprint(b));

    PlacementCell cell{
        "cell-001", "location-001", 100.f, 200.f, 0.f, 20.f, 30.f, true
    };
    assert(ResidentialBuildingPlanner::CanPlace(cell, 25.f));

    const auto targets =
        ResidentialBuildingPlanner::BuildTargetsForPopulation(1'000'000, 140);
    assert(targets.size() == 140);
    assert(targets.front().targetResidents > 0);

    return 0;
}
