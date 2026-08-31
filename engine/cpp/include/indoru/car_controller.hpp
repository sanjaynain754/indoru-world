#pragma once

#include "indoru/vehicle.hpp"

namespace indoru::vehicle {

struct CarCommand final {
    bool accelerate{false};
    bool reverse{false};
    bool brake{false};
    bool steer_left{false};
    bool steer_right{false};
    bool handbrake{false};
};

class CarController final {
public:
    explicit CarController(float input_response = 8.0F);

    void update(VehicleState& state, const CarCommand& command, const CarPhysics& physics, float delta_seconds);
    [[nodiscard]] const VehicleInput& current_input() const noexcept { return input_; }

private:
    float input_response_;
    VehicleInput input_{};
};

} // namespace indoru::vehicle
