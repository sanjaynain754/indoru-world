#include "indoru/engine.hpp"

#include <cassert>
#include <iostream>

int main() {
    indoru::engine::Engine engine("indoru-world-001");
    auto& world = engine.world();

    assert(world.register_country({"country-001", "Avenra", "flag-avenra"}));
    assert(world.set_active_country("country-001"));
    assert(world.register_settlement({1, "city-navaar", "Navaar", "country-001", indoru::world::SettlementType::Capital, {0.5F, 0.5F, 0.0F}, true}));
    assert(!world.register_settlement({2, "city-invalid", "Invalid", "country-missing", indoru::world::SettlementType::NormalCity, {}, false}));
    assert(world.set_weather({indoru::world::WeatherType::Rain, 20.0F, 0.8F, false}));

    engine.tick();
    const auto snapshot = world.snapshot();
    assert(snapshot.tick == 1);
    assert(snapshot.active_country_id == "country-001");
    assert(snapshot.settlements.size() == 1);
    assert(snapshot.settlements.front().name == "Navaar");

    std::cout << "Indoru world-state smoke test passed\n";
    return 0;
}
