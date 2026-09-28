#pragma once

#include "indoru/world_state.hpp"

#include <cstdint>
#include <span>
#include <string_view>

namespace indoru::world::content {

struct CountryContent final {
    std::string_view id;
    std::string_view name;
    std::string_view region_code;
    std::string_view flag_id;
    std::string_view flag_asset;
};

struct SettlementContent final {
    std::string_view id;
    std::string_view name;
    std::string_view country_id;
    std::string_view region_id;
    SettlementType type{SettlementType::Village};
    std::string_view facility_profile_id;
    std::string_view role;
};

struct FacilityProfileContent final {
    std::string_view id;
    std::span<const std::string_view> services;
};

struct ConnectivityRouteContent final {
    std::string_view mode;
    std::string_view name;
    std::span<const std::string_view> settlement_ids;
};

struct AirportRoleContent final {
    std::string_view settlement_id;
    std::string_view role;
};

struct RegionContent final {
    std::string_view id;
    std::string_view name;
    std::uint32_t build_order{0};
    bool runtime_integrated{false};
    bool ownership_assignments_proposed{true};
    std::span<const CountryContent> countries;
    std::span<const SettlementContent> settlements;
    std::span<const FacilityProfileContent> facility_profiles;
    std::span<const std::string_view> road_access_settlement_ids;
    std::span<const ConnectivityRouteContent> routes;
    std::span<const AirportRoleContent> airports;
};

/// Engine-neutral Avarra draft content compiled into the native library.
/// This view is design data only; it does not imply a playable renderer/runtime.
[[nodiscard]] const RegionContent& avarra_baseline_design() noexcept;

[[nodiscard]] bool has_service(
    const RegionContent& region,
    std::string_view facility_profile_id,
    std::string_view service_id) noexcept;

} // namespace indoru::world::content
