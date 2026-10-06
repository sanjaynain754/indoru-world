#include "indoru/city_building_framework.hpp"

namespace indoru {

bool CityBuildingFramework::ValidateRoad(const RoadSegment& road) {
    return !road.roadId.empty() && !road.locationId.empty()
        && road.lengthMeters > 0.0f && road.lanes >= 1
        && road.vehicleAccess;
}

bool CityBuildingFramework::ValidatePlot(const CityPlot& plot) {
    return !plot.plotId.empty() && !plot.locationId.empty()
        && plot.areaSquareMeters > 0.0f
        && plot.roadFrontage && plot.utilityReady;
}

std::uint32_t CityBuildingFramework::EstimatePlots(std::uint32_t buildings) {
    // Multiple building footprints can share a larger urban block.
    return buildings == 0 ? 0 : buildings + (buildings + 3) / 4;
}

CityInfrastructureTarget CityBuildingFramework::BuildTarget(
    const std::string& locationId, std::uint32_t buildings) {
    CityInfrastructureTarget t;
    t.locationId = locationId;
    t.roadSegments = buildings / 4 + 1;
    t.plots = EstimatePlots(buildings);
    t.utilityNodes = buildings / 20 + 1;
    t.zones = {
        CityZoneType::Government, CityZoneType::Police,
        CityZoneType::Medical, CityZoneType::Education,
        CityZoneType::Commerce, CityZoneType::Nightlife,
        CityZoneType::Residential, CityZoneType::Industrial,
        CityZoneType::Transport, CityZoneType::Culture,
        CityZoneType::Utility, CityZoneType::GreenSpace
    };
    return t;
}

} // namespace indoru
