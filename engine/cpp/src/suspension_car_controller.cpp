#include "indoru/suspension_car_controller.hpp"

#include <algorithm>
#include <cmath>

namespace indoru::vehicle {
namespace {

constexpr float kEpsilon = 0.0001F;

float dot(const world::Vec3& a, const world::Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

world::Vec3 add(const world::Vec3& a, const world::Vec3& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

world::Vec3 scale(const world::Vec3& value, float amount) {
    return {value.x * amount, value.y * amount, value.z * amount};
}

float length(const world::Vec3& value) {
    return std::sqrt(dot(value, value));
}

world::Vec3 normalize(const world::Vec3& value) {
    const float magnitude = length(value);
    return magnitude > kEpsilon ? scale(value, 1.0F / magnitude) : world::Vec3{};
}

world::Vec3 forward(float yaw) {
    return {std::sin(yaw), 0.0F, std::cos(yaw)};
}

world::Vec3 right(float yaw) {
    return {std::cos(yaw), 0.0F, -std::sin(yaw)};
}

world::Vec3 rotate_y(const world::Vec3& value, float yaw) {
    const float sine = std::sin(yaw);
    const float cosine = std::cos(yaw);
    return {value.x * cosine + value.z * sine, value.y, -value.x * sine + value.z * cosine};
}

} // namespace

SuspensionCarController::SuspensionCarController(SuspensionCarTuning tuning)
    : tuning_(tuning) {}

SuspensionCarStepResult SuspensionCarController::update(VehicleState& vehicle, WheelRig& rig,
                                                        const SuspensionCarInput& raw_input,
                                                        const TerrainCollider& terrain,
                                                        float delta_seconds) const {
    SuspensionCarStepResult result;
    if (!std::isfinite(delta_seconds) || delta_seconds <= 0.0F) {
        return result;
    }
    const float dt = std::min(delta_seconds, 0.05F);
    const float throttle = std::clamp(raw_input.throttle, -1.0F, 1.0F);
    const float brake = std::clamp(raw_input.brake, 0.0F, 1.0F);
    const float steering = std::clamp(raw_input.steering, -1.0F, 1.0F);
    result.suspension = WheelColliderSystem{}.solve(vehicle, rig, terrain, dt);

    const float mass = std::max(tuning_.mass_kg, 1.0F);
    const float steer_angle = steering * tuning_.max_steer_angle_rad;
    const float speed = length(vehicle.velocity);
    const world::Vec3 base_forward = forward(vehicle.yaw_rad);
    world::Vec3 total_force{};
    float yaw_torque = 0.0F;
    std::size_t grounded_count = 0;

    for (WheelState& wheel : rig.wheels) {
        if (!wheel.grounded) {
            continue;
        }
        ++grounded_count;
        const bool front_wheel = wheel.position == WheelPosition::FrontLeft || wheel.position == WheelPosition::FrontRight;
        const bool left_wheel = wheel.position == WheelPosition::FrontLeft || wheel.position == WheelPosition::RearLeft;
        const float wheel_yaw = front_wheel ? vehicle.yaw_rad + steer_angle : vehicle.yaw_rad;
        const world::Vec3 wheel_forward = forward(wheel_yaw);
        const world::Vec3 wheel_right = right(wheel_yaw);
        const float wheel_forward_speed = dot(vehicle.velocity, wheel_forward);
        const float wheel_lateral_speed = dot(vehicle.velocity, wheel_right);
        const float driven_factor = wheel.tuning.driven ? 1.0F : 0.0F;
        const float drive_force = throttle * tuning_.engine_force_n * driven_factor / 4.0F;
        const float brake_force = brake * tuning_.brake_force_n / 4.0F;
        const float signed_brake = wheel_forward_speed >= 0.0F ? -brake_force : brake_force;
        const float lateral_limit = std::max(wheel.normal_force_n * wheel.contact.friction * wheel.tuning.tire_grip, 0.0F);
        const float requested_lateral = -wheel_lateral_speed * tuning_.lateral_grip_n_per_mps;
        const float lateral_force = std::clamp(requested_lateral, -lateral_limit, lateral_limit);
        const float rolling_force = std::abs(wheel_forward_speed) > kEpsilon
            ? -std::copysign(tuning_.rolling_resistance_n / 4.0F, wheel_forward_speed)
            : 0.0F;
        const world::Vec3 wheel_force = add(
            scale(wheel_forward, drive_force + signed_brake + rolling_force),
            scale(wheel_right, raw_input.handbrake ? lateral_force * 0.35F : lateral_force));
        total_force = add(total_force, wheel_force);

        const world::Vec3 arm = rotate_y(wheel.local_anchor, vehicle.yaw_rad);
        yaw_torque += arm.x * wheel_force.z - arm.z * wheel_force.x;
        if (left_wheel) {
            yaw_torque += 0.0F;
        }
    }

    if (grounded_count > 0U) {
        const world::Vec3 base_direction = speed > kEpsilon ? normalize(vehicle.velocity) : base_forward;
        total_force = add(total_force, scale(base_direction, -tuning_.rolling_resistance_n));
    }

    const world::Vec3 acceleration = scale(total_force, 1.0F / mass);
    vehicle.velocity = add(vehicle.velocity, scale(acceleration, dt));
    const float new_speed = length(vehicle.velocity);
    if (new_speed > tuning_.max_speed_mps) {
        vehicle.velocity = scale(vehicle.velocity, tuning_.max_speed_mps / new_speed);
    }
    vehicle.position = add(vehicle.position, scale(vehicle.velocity, dt));
    vehicle.angular_velocity = yaw_torque / std::max(tuning_.yaw_inertia_kg_m2, 1.0F);
    vehicle.yaw_rad += vehicle.angular_velocity * dt;

    result.total_force = total_force;
    result.yaw_torque = yaw_torque;
    result.speed_mps = length(vehicle.velocity);
    return result;
}

} // namespace indoru::vehicle
