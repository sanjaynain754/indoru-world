#include "indoru/car_controller.hpp"

#include <algorithm>
#include <cmath>

namespace indoru::vehicle {
namespace {

float approach(float current, float target, float max_delta) {
    const float difference = target - current;
    if (std::abs(difference) <= max_delta) {
        return target;
    }
    return current + std::copysign(max_delta, difference);
}

} // namespace

CarController::CarController(float input_response)
    : input_response_(std::max(input_response, 0.0F)) {}

void CarController::update(VehicleState& state, const CarCommand& command, const CarPhysics& physics, float delta_seconds) {
    if (!std::isfinite(delta_seconds) || delta_seconds <= 0.0F) {
        return;
    }
    const float dt = std::min(delta_seconds, 0.05F);
    const float target_throttle = command.accelerate ? 1.0F : (command.reverse ? -1.0F : 0.0F);
    const float target_brake = command.brake ? 1.0F : 0.0F;
    const float target_steering = (command.steer_right ? 1.0F : 0.0F) - (command.steer_left ? 1.0F : 0.0F);
    const float response = input_response_ * dt;

    input_.throttle = approach(input_.throttle, target_throttle, response);
    input_.brake = approach(input_.brake, target_brake, response);
    input_.steering = approach(input_.steering, target_steering, response * 1.5F);
    input_.handbrake = command.handbrake;
    physics.step(state, input_, dt);
}

} // namespace indoru::vehicle
