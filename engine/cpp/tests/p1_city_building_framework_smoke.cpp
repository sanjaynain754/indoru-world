#include "indoru/city_building_framework.hpp"
#include <cassert>

using namespace indoru;

int main() {
    RoadSegment road{"road-001","navaar",RoadClass::Arterial,500.f,4,true,true};
    assert(CityBuildingFramework::ValidateRoad(road));

    CityPlot plot{"plot-001","navaar",CityZoneType::Residential,900.f,true,true};
    assert(CityBuildingFramework::ValidatePlot(plot));

    assert(CityBuildingFramework::EstimatePlots(100) == 125);

    auto target = CityBuildingFramework::BuildTarget("navaar", 1000);
    assert(target.roadSegments == 251);
    assert(target.plots == 1250);
    assert(target.utilityNodes == 51);
    assert(target.zones.size() == 12);

    return 0;
}
