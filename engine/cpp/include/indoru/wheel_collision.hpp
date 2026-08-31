#pragma once

#include "indoru/vehicle.hpp"

#include <array>
#include <cstddef>
#include <limits>

namespace indoru::vehicle {

struct Ray final {
    world::Vec3 origin{};
    world::Vec3 direction{0.0F, -1.0F, 0.0F};
    float length{1.0F};
};

struct RaycastHit final {
    bool hit{false};
    float distance{0.0F};
    world::Vec3 point{};
    world::Vec3 normal{0.0F, 1.0F, 0.0F};
    float friction{1.0F};
};

class TerrainCollider {
public:
    virtual ~TerrainCollider() = default;
    [[nodiscard]] virtual RaycastHit raycast(const Ray& ray) const = 0;
};

class HeightfieldTerrain final : public TerrainCollider {
public:
    explicit HeightfieldTerrain(float ground_height = 0.0F, float friction = 1.0F);
    [[nodiscard]] RaycastHit raycast(const Ray& ray) const override;

private:
    float ground_height_;
    float friction_;
};

enum class WheelPosition : std::uint8_t { FrontLeft, FrontRight, RearLeft, RearRight };

struct WheelTuning final {
    float radius_m{0.34F};
    float rest_length_m{0.34F};
    float travel_m{0.22F};
    float spring_rate_n_per_m{30000.0F};
    float damper_rate_n_s_per_m{4200.0F};
    float tire_grip{1.0F};
    bool steering{false};
    bool driven{true};
};

struct WheelState final {
    WheelPosition position{WheelPosition::FrontLeft};
    world::Vec3 local_anchor{};
    WheelTuning tuning{};
    float suspension_length_m{0.34F};
    float compression{0.0F};
    float previous_compression{0.0F};
    float normal_force_n{0.0F};
    bool grounded{false};
    RaycastHit contact{};
};

struct WheelRig final {
    std::array<WheelState, 4> wheels{};
};

struct SuspensionResult final {
    float total_normal_force_n{0.0F};
    std::size_t grounded_wheels{0};
    world::Vec3 average_contact_normal{0.0F, 1.0F, 0.0F};
};

class WheelColliderSystem final {
public:
    explicit WheelColliderSystem(WheelRig rig = {});

    SuspensionResult solve(VehicleState& vehicle, WheelRig& rig, const TerrainCollider& terrain, float delta_seconds) const;
    [[nodiscard]] static world::Vec3 world_point(const VehicleState& vehicle, const world::Vec3& local_point) noexcept;
    [[nodiscard]] static world::Vec3 rotate_y(const world::Vec3& value, float yaw) noexcept;

private:
    static void normalize_contact_normal(RaycastHit& hit);
};

} // namespace indoru::vehicle
