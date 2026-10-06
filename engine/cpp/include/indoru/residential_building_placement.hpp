#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace indoru {

enum class BuildingType : std::uint8_t {
    DetachedHouse,
    RowHouse,
    ApartmentLowRise,
    ApartmentMidRise,
    ApartmentHighRise,
    MixedUseResidential,
    Hostel,
    Hotel,
    ResidentialTower
};

struct BuildingFootprint {
    std::string buildingId;
    std::string countryId;
    std::string locationId;
    BuildingType type{BuildingType::DetachedHouse};
    std::uint16_t floors{1};
    std::uint16_t residentialUnits{1};
    std::uint16_t parkingSpaces{0};
    bool hasInterior{true};
    bool hasUtilityConnection{true};
};

struct PlacementCell {
    std::string cellId;
    std::string locationId;
    float x{0};
    float y{0};
    float rotationDegrees{0};
    float footprintWidth{0};
    float footprintDepth{0};
    bool roadAccess{false};
};

struct ResidentialBuildingTarget {
    std::string locationId;
    std::uint32_t targetUnits{0};
    std::uint32_t targetBuildings{0};
    std::uint32_t targetResidents{0};
    std::vector<BuildingType> allowedTypes;
};

class ResidentialBuildingPlanner {
public:
    static std::uint32_t EstimateBuildings(std::uint32_t targetUnits,
                                            std::uint16_t averageUnitsPerBuilding);

    static bool ValidateFootprint(const BuildingFootprint& building);

    static bool CanPlace(const PlacementCell& cell,
                         float minRoadAccessDistance);

    static std::vector<ResidentialBuildingTarget>
    BuildTargetsForPopulation(std::uint32_t population,
                              std::uint32_t locations);
};

} // namespace indoru
