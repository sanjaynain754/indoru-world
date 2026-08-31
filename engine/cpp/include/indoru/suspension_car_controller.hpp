#pragma once

#include "indoru/wheel_collision.hpp"

namespace indoru::vehicle {

struct SuspensionCarInput final {
    float throttle{0.0F};
    float brake{0.0F};
    float steering{0.0F};
    bool handbrake{false};
};

struct SuspensionCarTuning final {
    float mass_kg{1500.0F};
    float engine_force_n{8500.0F};
    float brake_force_n{12000.0F};
    float lateral_grip_n_per_mps{4200.0F};
    float rolling_resistance_n{180.0F};
    float max_steer_angle_rad{0.58F};
    float wheelbase_m{2.7F};
    float yaw_inertia_kg_m2{2500.0F};
    float max_speed_mps{75.0F};
};

struct SuspensionCarStepResult final {
    SuspensionResult suspension{};
    world::Vec3 total_force{};
    float yaw_torque{0.0F};
    float speed_mps{0.0F};
};

class SuspensionCarController final {
public:
    explicit SuspensionCarController(SuspensionCarTuning tuning = {});

    SuspensionCarStepResult update(VehicleState& vehicle, WheelRig& rig,
                                   const SuspensionCarInput& input,
                                   const TerrainCollider& terrain,
                                   float delta_seconds) const;

private:
    SuspensionCarTuning tuning_;
};

} // namespace indoru::vehicle
