#pragma once

#include <cstdint>
#include <optional>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace indoru::world {

using EntityId = std::uint64_t;
using Tick = std::uint64_t;

struct Vec3 final {
    float x{0.0F};
    float y{0.0F};
    float z{0.0F};
};

enum class SettlementType : std::uint8_t {
    Capital,
    MajorCity,
    NormalCity,
    Village,
};

enum class WeatherType : std::uint8_t {
    Clear,
    Rain,
    Snow,
    Fog,
    DustStorm,
    Storm,
};

struct CountryState final {
    std::string id;
    std::string name;
    std::string flag_ref;
};

struct SettlementState final {
    EntityId entity_id{0};
    std::string id;
    std::string name;
    std::string country_id;
    SettlementType type{SettlementType::Village};
    Vec3 map_position{};
    bool playable{false};
};

struct WeatherState final {
    WeatherType type{WeatherType::Clear};
    float temperature_c{22.0F};
    float visibility{1.0F};
    bool transport_disruption{false};
};

struct WorldSnapshot final {
    Tick tick{0};
    std::string world_id;
    std::string active_country_id;
    WeatherState weather{};
    std::vector<SettlementState> settlements;
};

class WorldState final {
public:
    explicit WorldState(std::string world_id);

    void advance_tick();
    [[nodiscard]] Tick current_tick() const noexcept;

    bool register_country(CountryState country);
    bool register_settlement(SettlementState settlement);
    bool set_weather(WeatherState weather);
    bool set_active_country(std::string country_id);

    [[nodiscard]] std::optional<CountryState> country(std::string_view id) const;
    [[nodiscard]] std::optional<SettlementState> settlement(std::string_view id) const;
    [[nodiscard]] WorldSnapshot snapshot() const;

private:
    mutable std::shared_mutex mutex_;
    Tick tick_{0};
    std::string world_id_;
    std::string active_country_id_;
    WeatherState weather_{};
    std::unordered_map<std::string, CountryState> countries_;
    std::unordered_map<std::string, SettlementState> settlements_;
};

} // namespace indoru::world
