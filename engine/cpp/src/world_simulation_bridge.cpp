#include "indoru/world_simulation_bridge.hpp"

#include <algorithm>

namespace indoru {

bool WorldSimulationBridge::ValidateTransportLink(const TransportLink& link) {
    if (link.linkId.empty() || link.locationId.empty() || link.lengthMeters <= 0.0f || link.lanesOrTracks == 0) {
        return false;
    }
    if (link.mode == TransportMode::River && !link.passengerService && !link.freightService) {
        return false;
    }
    return true;
}

CountrySimulationPlan WorldSimulationBridge::BuildPlan(
    const CountrySimulationInput& input,
    std::uint32_t buildingsPerLocation,
    std::uint16_t averageUnitsPerBuilding) {
    CountrySimulationPlan plan;
    plan.countryId = input.countryId;
    plan.population = input.population;
    if (input.countryId.empty() || input.locationIds.empty() || averageUnitsPerBuilding == 0) {
        return plan;
    }

    const auto targets = ResidentialBuildingPlanner::BuildTargetsForPopulation(
        input.population, static_cast<std::uint32_t>(input.locationIds.size()));
    plan.locations.reserve(input.locationIds.size());
    for (std::size_t index = 0; index < input.locationIds.size(); ++index) {
        LocationSimulationPlan location;
        location.locationId = input.locationIds[index];
        location.city = CityBuildingFramework::BuildTarget(location.locationId, buildingsPerLocation);
        location.residential = targets[index];
        for (const auto& link : input.transportLinks) {
            if (!ValidateTransportLink(link) || link.locationId != location.locationId) continue;
            ++plan.validTransportLinks;
            switch (link.mode) {
            case TransportMode::Road: ++location.roadLinks; break;
            case TransportMode::Rail: ++location.railLinks; break;
            case TransportMode::River: ++location.riverLinks; break;
            }
        }
        plan.locations.push_back(std::move(location));
    }
    return plan;
}

} // namespace indoru
