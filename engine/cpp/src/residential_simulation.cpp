#include "indoru/residential_simulation.hpp"
#include <algorithm>

namespace indoru {

NightDistribution ResidentialSimulation::CalculateNightDistribution(
    std::uint64_t population, double residentialShare) {
    residentialShare = std::clamp(residentialShare, 0.0, 1.0);
    auto residential = static_cast<std::uint64_t>(
        static_cast<double>(population) * residentialShare);
    return {
        population,
        residentialShare,
        residential,
        population - residential
    };
}

bool ResidentialSimulation::CanFitHousehold(const ResidentialUnit& unit,
                                             std::uint16_t currentOccupants,
                                             std::uint16_t additionalMembers) {
    const auto used = static_cast<std::uint32_t>(currentOccupants)
                    + static_cast<std::uint32_t>(additionalMembers);
    return used <= unit.capacity;
}

bool ResidentialSimulation::ValidateHousehold(const Household& household) {
    if (household.householdId.empty() ||
        household.residentialUnitId.empty() ||
        household.members.empty() ||
        !household.homeAssigned) {
        return false;
    }
    return household.members.size() <= 32;
}

HomeOccupancyState ResidentialSimulation::StateAtHour(
    const DailySchedule& schedule, std::uint8_t hour) {
    if (schedule.startHour == schedule.endHour) {
        return HomeOccupancyState::Home;
    }

    const bool crossesMidnight = schedule.startHour > schedule.endHour;
    const bool active = crossesMidnight
        ? (hour >= schedule.startHour || hour < schedule.endHour)
        : (hour >= schedule.startHour && hour < schedule.endHour);

    return active ? schedule.state : HomeOccupancyState::Home;
}

} // namespace indoru
