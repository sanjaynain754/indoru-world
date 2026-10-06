#pragma once

#include "indoru/city_building_framework.hpp"
#include "indoru/residential_building_placement.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace indoru {

enum class TransportMode : std::uint8_t {
    Road,
    Rail,
    River
};

struct TransportLink {
    std::string linkId;
    std::string locationId;
    TransportMode mode{TransportMode::Road};
    float lengthMeters{0.0f};
    std::uint16_t lanesOrTracks{1};
    bool passengerService{true};
    bool freightService{false};
};

struct CountrySimulationInput {
    std::string countryId;
    std::uint32_t population{0};
    std::vector<std::string> locationIds;
    std::vector<TransportLink> transportLinks;
};

struct LocationSimulationPlan {
    std::string locationId;
    CityInfrastructureTarget city;
    ResidentialBuildingTarget residential;
    std::uint32_t roadLinks{0};
    std::uint32_t railLinks{0};
    std::uint32_t riverLinks{0};
};

struct CountrySimulationPlan {
    std::string countryId;
    std::uint32_t population{0};
    std::vector<LocationSimulationPlan> locations;
    std::uint32_t validTransportLinks{0};
};

class WorldSimulationBridge {
public:
    static bool ValidateTransportLink(const TransportLink& link);

    static CountrySimulationPlan BuildPlan(const CountrySimulationInput& input,
                                           std::uint32_t buildingsPerLocation = 100,
                                           std::uint16_t averageUnitsPerBuilding = 8);
};

} // namespace indoru
