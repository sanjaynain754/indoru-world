#include "indoru/vehicle.hpp"

#include <algorithm>
#include <cmath>

namespace indoru::vehicle {
namespace {
constexpr float kEpsilon = 0.0001F;
constexpr float kGravity = 9.81F;

float clamp_input(float value) {
    return std::clamp(value, -1.0F, 1.0F);
}

world::Vec3 forward_vector(float yaw) {
    return {std::sin(yaw), 0.0F, std::cos(yaw)};
}

world::Vec3 right_vector(float yaw) {
    return {std::cos(yaw), 0.0F, -std::sin(yaw)};
}

float dot(const world::Vec3& a, const world::Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

world::Vec3 scale(const world::Vec3& value, float factor) {
    return {value.x * factor, value.y * factor, value.z * factor};
}

world::Vec3 add(const world::Vec3& a, const world::Vec3& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

} // namespace

CarPhysics::CarPhysics(VehicleTuning tuning)
    : tuning_(tuning) {}

void CarPhysics::step(VehicleState& state, const VehicleInput& raw_input, float delta_seconds) const {
    if (!std::isfinite(delta_seconds) || delta_seconds <= 0.0F) {
        return;
    }
    const float dt = std::min(delta_seconds, 0.05F);
    const float throttle = clamp_input(raw_input.throttle);
    const float brake = std::clamp(raw_input.brake, 0.0F, 1.0F);
    const float steering = clamp_input(raw_input.steering);
    const world::Vec3 forward = forward_vector(state.yaw_rad);
    const world::Vec3 right = right_vector(state.yaw_rad);

    const float forward_speed = dot(state.velocity, forward);
    const float lateral_speed = dot(state.velocity, right);
    const float drive_force = throttle >= 0.0F
        ? throttle * tuning_.engine_force_n
        : throttle * tuning_.reverse_force_n;
    const float brake_direction = forward_speed >= 0.0F ? -1.0F : 1.0F;
    const float brake_force = brake * tuning_.brake_force_n * brake_direction;
    const float rolling_force = std::abs(forward_speed) > kEpsilon
        ? -std::copysign(tuning_.rolling_resistance_n, forward_speed)
        : 0.0F;
    const float drag_force = -forward_speed * std::abs(forward_speed) * tuning_.aerodynamic_drag;
    const float longitudinal_force = drive_force + brake_force + rolling_force + drag_force;
    const float longitudinal_acceleration = longitudinal_force / std::max(tuning_.mass_kg, 1.0F);

    const float grip = raw_input.handbrake ? tuning_.handbrake_grip : tuning_.lateral_grip;
    const float lateral_acceleration = -lateral_speed * grip;
    const world::Vec3 acceleration = add(
        scale(forward, longitudinal_acceleration),
        scale(right, lateral_acceleration));

    state.velocity = add(state.velocity, scale(acceleration, dt));
    const float speed = std::sqrt(dot(state.velocity, state.velocity));
    if (speed > tuning_.max_speed_mps) {
        state.velocity = scale(state.velocity, tuning_.max_speed_mps / speed);
    }

    const float steer_angle = steering * tuning_.max_steer_angle_rad;
    const float speed_factor = std::clamp(std::abs(forward_speed) / 8.0F, 0.0F, 1.0F);
    state.angular_velocity = std::tan(steer_angle) * forward_speed / std::max(tuning_.wheelbase_m, 0.1F) * speed_factor;
    if (!state.grounded) {
        state.angular_velocity *= 0.2F;
    }
    state.yaw_rad += state.angular_velocity * dt;
    state.position = add(state.position, scale(state.velocity, dt));
}

} // namespace indoru::vehicle
