#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace indoru {

enum class StateLevel : std::uint8_t {
    World,
    Region,
    Country,
    City,
    Settlement
};

struct StateRecord {
    std::string stateId;
    std::string parentStateId;
    std::string countryId;
    std::string name;
    StateLevel level{StateLevel::Settlement};
    std::uint64_t population{0};
    bool playable{true};
};

struct StatePopulationDelta {
    std::string stateId;
    std::int64_t delta{0};
};

class StateSystem {
public:
    static bool ValidateRecord(const StateRecord& state);

    static bool ValidateHierarchy(const std::string& worldId,
                                  const std::vector<StateRecord>& states);

    static bool ApplyPopulationDelta(StateRecord& state,
                                     std::int64_t delta);
};

} // namespace indoru
