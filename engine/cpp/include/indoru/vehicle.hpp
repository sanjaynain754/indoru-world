#pragma once

#include "indoru/world_state.hpp"

#include <algorithm>
#include <cmath>

namespace indoru::vehicle {

struct VehicleInput final {
    float throttle{0.0F};  // -1 reverse, 0 neutral, +1 accelerate
    float brake{0.0F};     // 0..1
    float steering{0.0F};  // -1 left, 0 center, +1 right
    bool handbrake{false};
};

struct VehicleTuning final {
    float mass_kg{1500.0F};
    float engine_force_n{8500.0F};
    float reverse_force_n{3500.0F};
    float brake_force_n{12000.0F};
    float rolling_resistance_n{180.0F};
    float aerodynamic_drag{0.38F};
    float max_steer_angle_rad{0.58F};
    float wheelbase_m{2.7F};
    float lateral_grip{8.0F};
    float handbrake_grip{1.8F};
    float max_speed_mps{75.0F};
};

struct VehicleState final {
    world::Vec3 position{};
    world::Vec3 velocity{};
    float yaw_rad{0.0F};
    float roll_rad{0.0F};
    float pitch_rad{0.0F};
    float angular_velocity{0.0F};
    float roll_velocity{0.0F};
    float pitch_velocity{0.0F};
    bool grounded{true};
};

class CarPhysics final {
public:
    explicit CarPhysics(VehicleTuning tuning = {});

    void step(VehicleState& state, const VehicleInput& input, float delta_seconds) const;
    [[nodiscard]] const VehicleTuning& tuning() const noexcept { return tuning_; }

private:
    VehicleTuning tuning_;
};

} // namespace indoru::vehicle
