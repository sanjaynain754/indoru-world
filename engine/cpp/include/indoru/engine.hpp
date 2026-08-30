#pragma once

#include "indoru/world_state.hpp"

#include <cstdint>
#include <memory>
#include <string_view>

namespace indoru::engine {

class Engine final {
public:
    explicit Engine(std::string_view world_id);

    void tick();
    [[nodiscard]] const world::WorldState& world() const noexcept;
    [[nodiscard]] world::WorldState& world() noexcept;

private:
    std::unique_ptr<world::WorldState> world_;
};

} // namespace indoru::engine
