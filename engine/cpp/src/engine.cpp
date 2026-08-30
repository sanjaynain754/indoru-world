#include "indoru/engine.hpp"

#include <string>

namespace indoru::engine {

Engine::Engine(std::string_view world_id)
    : world_(std::make_unique<world::WorldState>(std::string(world_id))) {}

void Engine::tick() {
    world_->advance_tick();
}

const world::WorldState& Engine::world() const noexcept {
    return *world_;
}

world::WorldState& Engine::world() noexcept {
    return *world_;
}

} // namespace indoru::engine
