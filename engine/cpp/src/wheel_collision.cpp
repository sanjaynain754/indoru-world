#include "indoru/wheel_collision.hpp"

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
    return magnitude > kEpsilon ? scale(value, 1.0F / magnitude) : world::Vec3{0.0F, 1.0F, 0.0F};
}

} // namespace

HeightfieldTerrain::HeightfieldTerrain(float ground_height, float friction)
    : ground_height_(ground_height), friction_(std::clamp(friction, 0.0F, 2.0F)) {}

RaycastHit HeightfieldTerrain::raycast(const Ray& ray) const {
    RaycastHit result;
    const world::Vec3 direction = normalize(ray.direction);
    if (ray.length <= 0.0F || std::abs(direction.y) < kEpsilon || direction.y >= 0.0F) {
        return result;
    }
    const float t = (ground_height_ - ray.origin.y) / direction.y;
    if (t < 0.0F || t > ray.length) {
        return result;
    }
    result.hit = true;
    result.distance = t;
    result.point = add(ray.origin, scale(direction, t));
    result.normal = {0.0F, 1.0F, 0.0F};
    result.friction = friction_;
    return result;
}

WheelColliderSystem::WheelColliderSystem(WheelRig) {}

world::Vec3 WheelColliderSystem::rotate_y(const world::Vec3& value, float yaw) noexcept {
    const float sine = std::sin(yaw);
    const float cosine = std::cos(yaw);
    return {value.x * cosine + value.z * sine, value.y, -value.x * sine + value.z * cosine};
}

world::Vec3 WheelColliderSystem::world_point(const VehicleState& vehicle, const world::Vec3& local_point) noexcept {
    return add(vehicle.position, rotate_y(local_point, vehicle.yaw_rad));
}

void WheelColliderSystem::normalize_contact_normal(RaycastHit& hit) {
    hit.normal = normalize(hit.normal);
    hit.friction = std::clamp(hit.friction, 0.0F, 2.0F);
}

SuspensionResult WheelColliderSystem::solve(VehicleState& vehicle, WheelRig& rig, const TerrainCollider& terrain, float delta_seconds) const {
    SuspensionResult result;
    if (!std::isfinite(delta_seconds) || delta_seconds <= 0.0F) {
        return result;
    }
    const float dt = std::min(delta_seconds, 0.05F);
    world::Vec3 normal_sum{};

    for (WheelState& wheel : rig.wheels) {
        const float max_length = std::max(wheel.tuning.rest_length_m + wheel.tuning.travel_m, 0.01F);
        const world::Vec3 anchor = world_point(vehicle, wheel.local_anchor);
        const Ray ray{anchor, {0.0F, -1.0F, 0.0F}, max_length + wheel.tuning.radius_m};
        wheel.previous_compression = wheel.compression;
        wheel.contact = terrain.raycast(ray);
        if (!wheel.contact.hit) {
            wheel.grounded = false;
            wheel.compression = 0.0F;
            wheel.normal_force_n = 0.0F;
            continue;
        }
        normalize_contact_normal(wheel.contact);
        wheel.grounded = true;
        const float desired_length = std::clamp(wheel.contact.distance - wheel.tuning.radius_m, 0.0F, max_length);
        wheel.suspension_length_m = desired_length;
        wheel.compression = std::clamp((max_length - desired_length) / max_length, 0.0F, 1.0F);
        const float compression_velocity = (wheel.compression - wheel.previous_compression) / dt;
        const float spring_force = wheel.compression * wheel.tuning.spring_rate_n_per_m;
        const float damper_force = -compression_velocity * wheel.tuning.damper_rate_n_s_per_m;
        wheel.normal_force_n = std::max(0.0F, spring_force + damper_force);
        result.total_normal_force_n += wheel.normal_force_n;
        normal_sum = add(normal_sum, scale(wheel.contact.normal, wheel.normal_force_n));
        ++result.grounded_wheels;
    }

    vehicle.grounded = result.grounded_wheels > 0;
    if (result.total_normal_force_n > kEpsilon) {
        result.average_contact_normal = normalize(normal_sum);
    }
    return result;
}

} // namespace indoru::vehicle
