#include "indoru/region_content.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string_view>

int main() {
    using namespace indoru::world;
    using namespace indoru::world::content;

    const auto& region = avarra_baseline_design();
    assert(region.id == "region-avarra-crescent");
    assert(region.name == "Avarra Crescent");
    assert(region.build_order == 1U);
    assert(!region.runtime_integrated);
    assert(region.ownership_assignments_proposed);
    assert(region.countries.size() == 20U);
    assert(region.settlements.size() == 36U);

    for (const auto& settlement : region.settlements) {
        const auto owner = std::find_if(region.countries.begin(), region.countries.end(),
            [&settlement](const CountryContent& country) { return country.id == settlement.country_id; });
        assert(owner != region.countries.end());
        assert(settlement.region_id == region.id);
        if (settlement.type == SettlementType::Village) {
            assert(has_service(region, settlement.facility_profile_id, "mobile_barber_service"));
            assert(has_service(region, settlement.facility_profile_id, "potable_water_point"));
        } else {
            assert(has_service(region, settlement.facility_profile_id, "barber_shop"));
            assert(has_service(region, settlement.facility_profile_id, "public_market"));
        }
    }

    for (const auto& country : region.countries) {
        const auto covered = std::any_of(region.settlements.begin(), region.settlements.end(),
            [&country](const SettlementContent& settlement) { return settlement.country_id == country.id; });
        assert(covered);
    }

    for (const auto settlement_id : region.road_access_settlement_ids) {
        const auto exists = std::any_of(region.settlements.begin(), region.settlements.end(),
            [settlement_id](const SettlementContent& settlement) { return settlement.id == settlement_id; });
        assert(exists);
    }
    assert(region.road_access_settlement_ids.size() == region.settlements.size());

    for (const auto& route : region.routes) {
        assert(route.settlement_ids.size() >= 2U);
        for (const auto settlement_id : route.settlement_ids) {
            const auto exists = std::any_of(region.settlements.begin(), region.settlements.end(),
                [settlement_id](const SettlementContent& settlement) { return settlement.id == settlement_id; });
            assert(exists);
        }
    }

    assert(region.airports.size() == 3U);
    for (const auto& airport : region.airports) {
        const auto exists = std::any_of(region.settlements.begin(), region.settlements.end(),
            [&airport](const SettlementContent& settlement) { return settlement.id == airport.settlement_id; });
        assert(exists);
    }

    std::cout << "Avarra compiled content smoke test passed (design baseline; not playable runtime)\n";
    return 0;
}
