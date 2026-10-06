#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace indoru {

enum class CityZoneType : std::uint8_t {
    Government, Police, Medical, Education, Commerce, Nightlife,
    Residential, Industrial, Transport, Culture, Utility, GreenSpace
};

enum class RoadClass : std::uint8_t {
    Local, Collector, Arterial, Highway
};

struct RoadSegment {
    std::string roadId;
    std::string locationId;
    RoadClass classType{RoadClass::Local};
    float lengthMeters{0};
    std::uint16_t lanes{2};
    bool pedestrianAccess{true};
    bool vehicleAccess{true};
};

struct CityPlot {
    std::string plotId;
    std::string locationId;
    CityZoneType zone{CityZoneType::Residential};
    float areaSquareMeters{0};
    bool roadFrontage{false};
    bool utilityReady{false};
};

struct CityInfrastructureTarget {
    std::string locationId;
    std::uint32_t roadSegments{0};
    std::uint32_t plots{0};
    std::uint32_t utilityNodes{0};
    std::vector<CityZoneType> zones;
};

class CityBuildingFramework {
public:
    static bool ValidateRoad(const RoadSegment& road);
    static bool ValidatePlot(const CityPlot& plot);
    static std::uint32_t EstimatePlots(std::uint32_t buildings);
    static CityInfrastructureTarget BuildTarget(
        const std::string& locationId,
        std::uint32_t buildings);
};

} // namespace indoru
