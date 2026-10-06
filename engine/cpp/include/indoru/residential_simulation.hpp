#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace indoru {

enum class HouseholdType : std::uint8_t {
    Single,
    Couple,
    SmallFamily,
    LargeFamily,
    JointFamily,
    SharedResidence
};

enum class HomeOccupancyState : std::uint8_t {
    Home,
    Work,
    School,
    Shopping,
    Leisure,
    NightShift,
    Emergency,
    Travelling
};

struct HouseholdMember {
    std::string npcId;
    std::string role;
    std::uint8_t age{0};
};

struct ResidentialUnit {
    std::string unitId;
    std::string buildingId;
    std::string locationId;
    std::string countryId;
    std::uint16_t capacity{1};
    std::vector<std::string> householdIds;
};

struct Household {
    std::string householdId;
    HouseholdType type{HouseholdType::SmallFamily};
    std::string residentialUnitId;
    std::vector<HouseholdMember> members;
    bool homeAssigned{false};
};

struct DailySchedule {
    std::string npcId;
    std::string householdId;
    HomeOccupancyState state{HomeOccupancyState::Home};
    std::uint8_t startHour{0};
    std::uint8_t endHour{24};
    std::string destinationId;
};

struct NightDistribution {
    std::uint64_t totalPopulation{0};
    double residentialShare{0.90};
    std::uint64_t residentialPopulation{0};
    std::uint64_t nonResidentialPopulation{0};
};

class ResidentialSimulation {
public:
    static NightDistribution CalculateNightDistribution(std::uint64_t population,
                                                        double residentialShare = 0.90);

    static bool CanFitHousehold(const ResidentialUnit& unit,
                                std::uint16_t currentOccupants,
                                std::uint16_t additionalMembers);

    static bool ValidateHousehold(const Household& household);

    static HomeOccupancyState StateAtHour(const DailySchedule& schedule,
                                          std::uint8_t hour);
};

} // namespace indoru
