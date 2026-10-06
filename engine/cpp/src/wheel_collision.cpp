#include "indoru/wheel_collision.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

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

HeightfieldTerrain::HeightfieldTerrain(std::vector<float> heights, std::size_t columns,
                                       std::size_t rows, float cell_size_m, float origin_x_m,
                                       float origin_z_m, float friction)
    : ground_height_(0.0F), friction_(std::clamp(friction, 0.0F, 2.0F)),
      heights_(std::move(heights)), columns_(columns), rows_(rows),
      cell_size_m_(cell_size_m), origin_x_m_(origin_x_m), origin_z_m_(origin_z_m) {
    const bool valid_shape = columns_ >= 2U && rows_ >= 2U &&
                             heights_.size() == columns_ * rows_ &&
                             std::isfinite(cell_size_m_) && cell_size_m_ > kEpsilon;
    if (!valid_shape) {
        heights_.clear();
        columns_ = 0U;
        rows_ = 0U;
    }
}

RaycastHit HeightfieldTerrain::raycast(const Ray& ray) const {
    RaycastHit result;
    const world::Vec3 direction = normalize(ray.direction);
    if (!std::isfinite(ray.length) || ray.length <= 0.0F ||
        std::abs(direction.y) < kEpsilon || direction.y >= 0.0F) {
        return result;
    }
    float terrain_height = ground_height_;
    world::Vec3 terrain_normal{0.0F, 1.0F, 0.0F};
    if (!heights_.empty()) {
        // Wheel suspension currently issues vertical rays. Rejecting oblique
        // queries avoids pretending a height sample is a mesh intersection.
        if (std::abs(direction.x) > kEpsilon || std::abs(direction.z) > kEpsilon) {
            return result;
        }
        const float grid_x = (ray.origin.x - origin_x_m_) / cell_size_m_;
        const float grid_z = (ray.origin.z - origin_z_m_) / cell_size_m_;
        const float max_x = static_cast<float>(columns_ - 1U);
        const float max_z = static_cast<float>(rows_ - 1U);
        if (!std::isfinite(grid_x) || !std::isfinite(grid_z) || grid_x < 0.0F ||
            grid_z < 0.0F || grid_x > max_x || grid_z > max_z) {
            return result;
        }
        const auto sample = [this](std::size_t x, std::size_t z) {
            return heights_[z * columns_ + x];
        };
        const std::size_t x0 = std::min(static_cast<std::size_t>(grid_x), columns_ - 2U);
        const std::size_t z0 = std::min(static_cast<std::size_t>(grid_z), rows_ - 2U);
        const float tx = std::clamp(grid_x - static_cast<float>(x0), 0.0F, 1.0F);
        const float tz = std::clamp(grid_z - static_cast<float>(z0), 0.0F, 1.0F);
        const float h00 = sample(x0, z0);
        const float h10 = sample(x0 + 1U, z0);
        const float h01 = sample(x0, z0 + 1U);
        const float h11 = sample(x0 + 1U, z0 + 1U);
        const float hx0 = h00 + (h10 - h00) * tx;
        const float hx1 = h01 + (h11 - h01) * tx;
        terrain_height = hx0 + (hx1 - hx0) * tz;
        const float slope_x = ((h10 - h00) * (1.0F - tz) + (h11 - h01) * tz) / cell_size_m_;
        const float slope_z = ((h01 - h00) * (1.0F - tx) + (h11 - h10) * tx) / cell_size_m_;
        terrain_normal = normalize({-slope_x, 1.0F, -slope_z});
    }
    const float t = (terrain_height - ray.origin.y) / direction.y;
    if (t < 0.0F || t > ray.length) {
        return result;
    }
    result.hit = true;
    result.distance = t;
    result.point = add(ray.origin, scale(direction, t));
    result.normal = terrain_normal;
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
        const bool was_grounded = wheel.grounded;
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
        const float compression_velocity = was_grounded
                                               ? (wheel.compression - wheel.previous_compression) / dt
                                               : 0.0F;
        const float spring_force = wheel.compression * wheel.tuning.spring_rate_n_per_m;
        const float damper_force = compression_velocity * wheel.tuning.damper_rate_n_s_per_m;
        wheel.normal_force_n = std::max(0.0F, spring_force + damper_force);
        result.total_normal_force_n += wheel.normal_force_n;
        normal_sum = add(normal_sum, scale(wheel.contact.normal, wheel.normal_force_n));
        result.total_contact_force = add(result.total_contact_force,
                                         scale(wheel.contact.normal, wheel.normal_force_n));
        ++result.grounded_wheels;
    }

    vehicle.grounded = result.grounded_wheels > 0;
    if (result.total_normal_force_n > kEpsilon) {
        result.average_contact_normal = normalize(normal_sum);
    }
    return result;
}

} // namespace indoru::vehicle
