#pragma once

#include <algorithm>
#include <cmath>

namespace indoru::vehicle {

struct PacejkaCoefficients final {
    float longitudinal_b{10.0F};
    float longitudinal_c{1.9F};
    float longitudinal_d{1.0F};
    float longitudinal_e{0.97F};
    float lateral_b{7.5F};
    float lateral_c{1.3F};
    float lateral_d{1.0F};
    float lateral_e{0.8F};
    float reference_load_n{3500.0F};
    float load_sensitivity{0.12F};
    float combined_exponent{2.0F};
};

struct TireInput final {
    float longitudinal_slip{0.0F}; // positive = driven wheel, negative = braking
    float slip_angle_rad{0.0F};    // positive = velocity points left of wheel heading
    float normal_load_n{0.0F};
    float surface_friction{1.0F};
    float tire_grip{1.0F};
};

struct TireForce final {
    float longitudinal_n{0.0F};
    float lateral_n{0.0F};
    float utilization{0.0F};
};

class PacejkaTire final {
public:
    explicit PacejkaTire(PacejkaCoefficients coefficients = {});

    [[nodiscard]] TireForce evaluate(const TireInput& input) const noexcept;
    [[nodiscard]] float longitudinal_curve(float slip, float load_n) const noexcept;
    [[nodiscard]] float lateral_curve(float slip_angle_rad, float load_n) const noexcept;

private:
    [[nodiscard]] float magic_formula(float slip, float b, float c, float d, float e) const noexcept;
    [[nodiscard]] float load_scale(float load_n) const noexcept;
    PacejkaCoefficients coefficients_;
};

} // namespace indoru::vehicle
