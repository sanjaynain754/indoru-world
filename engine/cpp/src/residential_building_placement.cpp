#include "indoru/residential_building_placement.hpp"
#include <algorithm>
#include <cmath>

namespace indoru {

std::uint32_t ResidentialBuildingPlanner::EstimateBuildings(
    std::uint32_t targetUnits, std::uint16_t averageUnitsPerBuilding) {
    if (targetUnits == 0 || averageUnitsPerBuilding == 0) return 0;
    return (targetUnits + averageUnitsPerBuilding - 1) / averageUnitsPerBuilding;
}

bool ResidentialBuildingPlanner::ValidateFootprint(
    const BuildingFootprint& building) {
    return !building.buildingId.empty()
        && !building.countryId.empty()
        && !building.locationId.empty()
        && building.floors >= 1
        && building.residentialUnits >= 1
        && building.hasInterior
        && building.hasUtilityConnection;
}

bool ResidentialBuildingPlanner::CanPlace(
    const PlacementCell& cell, float minRoadAccessDistance) {
    if (cell.cellId.empty() || cell.locationId.empty()) return false;
    if (cell.footprintWidth <= 0 || cell.footprintDepth <= 0) return false;
    // A production UE5 implementation will replace this with spline/nav/road queries.
    return cell.roadAccess || minRoadAccessDistance <= 0.0f;
}

std::vector<ResidentialBuildingTarget>
ResidentialBuildingPlanner::BuildTargetsForPopulation(
    std::uint32_t population, std::uint32_t locations) {
    if (population == 0 || locations == 0) return {};
    const std::uint32_t avgHouseholdSize = 4;
    const std::uint32_t targetUnits =
        (population + avgHouseholdSize - 1) / avgHouseholdSize;
    const std::uint32_t perLocation =
        (targetUnits + locations - 1) / locations;

    std::vector<ResidentialBuildingTarget> out;
    out.reserve(locations);

    for (std::uint32_t i = 0; i < locations; ++i) {
        ResidentialBuildingTarget t;
        t.locationId = "location-" + std::to_string(i + 1);
        t.targetUnits = perLocation;
        t.targetResidents = population / locations +
            (i < population % locations ? 1 : 0);
        // Mix is intentionally diversified; final city-specific ratios come from UE5 data.
        t.allowedTypes = {
            BuildingType::DetachedHouse,
            BuildingType::RowHouse,
            BuildingType::ApartmentLowRise,
            BuildingType::ApartmentMidRise,
            BuildingType::MixedUseResidential
        };
        t.targetBuildings = EstimateBuildings(t.targetUnits, 8);
        out.push_back(std::move(t));
    }
    return out;
}

} // namespace indoru
