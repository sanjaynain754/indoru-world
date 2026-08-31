#include "indoru/car_controller.hpp"
#include "indoru/vehicle.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    indoru::vehicle::CarPhysics physics;
    indoru::vehicle::VehicleState state{};
    indoru::vehicle::VehicleInput input{};

    input.throttle = 1.0F;
    for (int i = 0; i < 120; ++i) {
        physics.step(state, input, 1.0F / 60.0F);
    }
    const float accelerated_speed = std::sqrt(
        state.velocity.x * state.velocity.x + state.velocity.z * state.velocity.z);
    assert(accelerated_speed > 1.0F);
    assert(accelerated_speed <= physics.tuning().max_speed_mps + 0.01F);

    const float speed_before_braking = accelerated_speed;
    input.throttle = 0.0F;
    input.brake = 1.0F;
    for (int i = 0; i < 60; ++i) {
        physics.step(state, input, 1.0F / 60.0F);
    }
    const float speed_after_braking = std::sqrt(
        state.velocity.x * state.velocity.x + state.velocity.z * state.velocity.z);
    assert(speed_after_braking < speed_before_braking);

    indoru::vehicle::CarController controller;
    indoru::vehicle::VehicleState controlled_state{};
    indoru::vehicle::CarCommand command{};
    command.accelerate = true;
    command.steer_left = true;
    for (int i = 0; i < 120; ++i) {
        controller.update(controlled_state, command, physics, 1.0F / 60.0F);
    }
    assert(controlled_state.position.z > 0.0F);
    assert(controlled_state.yaw_rad < 0.0F);

    std::cout << "Indoru vehicle physics smoke test passed\n";
    return 0;
}
