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

bool is_front(WheelPosition position) {
    return position == WheelPosition::FrontLeft || position == WheelPosition::FrontRight;
}

bool is_left(WheelPosition position) {
    return position == WheelPosition::FrontLeft || position == WheelPosition::RearLeft;
}

} // namespace

SuspensionCarController::SuspensionCarController(SuspensionCarTuning tuning)
    : tuning_(tuning), tire_({}) {}

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
    float front_left_compression = 0.0F;
    float front_right_compression = 0.0F;
    float rear_left_compression = 0.0F;
    float rear_right_compression = 0.0F;

    for (const WheelState& wheel : rig.wheels) {
        if (!wheel.grounded) {
            continue;
        }
        ++grounded_count;
        switch (wheel.position) {
            case WheelPosition::FrontLeft: front_left_compression = wheel.compression; break;
            case WheelPosition::FrontRight: front_right_compression = wheel.compression; break;
            case WheelPosition::RearLeft: rear_left_compression = wheel.compression; break;
            case WheelPosition::RearRight: rear_right_compression = wheel.compression; break;
        }
    }

    const float front_anti_roll = (front_left_compression - front_right_compression) * tuning_.front_anti_roll_n_per_compression;
    const float rear_anti_roll = (rear_left_compression - rear_right_compression) * tuning_.rear_anti_roll_n_per_compression;

    for (WheelState& wheel : rig.wheels) {
        if (!wheel.grounded) {
            continue;
        }
        const bool front_wheel = is_front(wheel.position);
        const bool left_wheel = is_left(wheel.position);
        const float wheel_yaw = front_wheel ? vehicle.yaw_rad + steer_angle : vehicle.yaw_rad;
        const world::Vec3 wheel_forward = forward(wheel_yaw);
        const world::Vec3 wheel_right = right(wheel_yaw);
        const float wheel_forward_speed = dot(vehicle.velocity, wheel_forward);
        const float wheel_lateral_speed = dot(vehicle.velocity, wheel_right);
        const float driven_factor = wheel.tuning.driven ? 1.0F : 0.0F;
        const float wheel_radius = std::max(wheel.tuning.radius_m, 0.05F);
        const float wheel_angular_speed = (throttle * tuning_.max_speed_mps / wheel_radius) * driven_factor;
        const float contact_speed = std::max(std::abs(wheel_forward_speed), 0.5F);
        const float longitudinal_slip = (wheel_angular_speed * wheel_radius - wheel_forward_speed) / contact_speed;
        const float slip_angle = std::atan2(wheel_lateral_speed, std::abs(wheel_forward_speed) + 0.5F);
        const float drive_brake_scale = throttle * tuning_.engine_force_n * driven_factor / std::max(wheel.normal_force_n, 1.0F);
        const float brake_scale = brake * tuning_.brake_force_n / std::max(wheel.normal_force_n, 1.0F);
        const float requested_longitudinal = longitudinal_slip + drive_brake_scale * 0.12F - brake_scale * 0.12F;
        const TireForce tire_force = tire_.evaluate({requested_longitudinal, slip_angle, wheel.normal_force_n,
                                                       wheel.contact.friction, wheel.tuning.tire_grip});
        float longitudinal_force = tire_force.longitudinal_n;
        if (std::abs(wheel_forward_speed) > kEpsilon) {
            longitudinal_force -= std::copysign(tuning_.rolling_resistance_n / 4.0F, wheel_forward_speed);
        }
        float lateral_force = tire_force.lateral_n;
        if (raw_input.handbrake) {
            lateral_force *= 0.35F;
        }
        float anti_roll_force = front_wheel ? front_anti_roll : rear_anti_roll;
        if (!left_wheel) {
            anti_roll_force = -anti_roll_force;
        }
        const world::Vec3 wheel_force = add(
            add(scale(wheel_forward, longitudinal_force), scale(wheel_right, lateral_force)),
            scale(wheel.contact.normal, anti_roll_force));
        total_force = add(total_force, wheel_force);
        const world::Vec3 arm = rotate_y(wheel.local_anchor, vehicle.yaw_rad);
        yaw_torque += arm.x * wheel_force.z - arm.z * wheel_force.x;
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

    const float longitudinal_acceleration = dot(acceleration, base_forward);
    const float lateral_acceleration = dot(acceleration, right(vehicle.yaw_rad));
    const float target_roll_torque = lateral_acceleration * mass * 0.62F - vehicle.roll_rad * tuning_.roll_stiffness_n_m_per_rad - vehicle.roll_velocity * tuning_.roll_damping_n_m_s_per_rad;
    const float target_pitch_torque = -longitudinal_acceleration * mass * 0.48F - vehicle.pitch_rad * tuning_.pitch_stiffness_n_m_per_rad - vehicle.pitch_velocity * tuning_.pitch_damping_n_m_s_per_rad;
    const float roll_torque = target_roll_torque;
    const float pitch_torque = target_pitch_torque;

    vehicle.roll_velocity += roll_torque / std::max(tuning_.roll_inertia_kg_m2, 1.0F) * dt;
    vehicle.pitch_velocity += pitch_torque / std::max(tuning_.pitch_inertia_kg_m2, 1.0F) * dt;
    vehicle.roll_rad = std::clamp(vehicle.roll_rad + vehicle.roll_velocity * dt, -tuning_.max_roll_rad, tuning_.max_roll_rad);
    vehicle.pitch_rad = std::clamp(vehicle.pitch_rad + vehicle.pitch_velocity * dt, -tuning_.max_pitch_rad, tuning_.max_pitch_rad);
    vehicle.position = add(vehicle.position, scale(vehicle.velocity, dt));
    vehicle.angular_velocity = yaw_torque / std::max(tuning_.yaw_inertia_kg_m2, 1.0F);
    vehicle.yaw_rad += vehicle.angular_velocity * dt;

    result.total_force = total_force;
    result.yaw_torque = yaw_torque;
    result.roll_torque = roll_torque;
    result.pitch_torque = pitch_torque;
    result.speed_mps = length(vehicle.velocity);
    return result;
}

} // namespace indoru::vehicle
