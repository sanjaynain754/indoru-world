#include "indoru/tire_model.hpp"

#include <algorithm>
#include <cmath>

namespace indoru::vehicle {
namespace {
constexpr float kEpsilon = 0.0001F;
constexpr float kHalfPi = 1.57079632679F;
}

PacejkaTire::PacejkaTire(PacejkaCoefficients coefficients)
    : coefficients_(coefficients) {}

float PacejkaTire::magic_formula(float slip, float b, float c, float d, float e) const noexcept {
    const float bx = b * slip;
    return d * std::sin(c * std::atan(bx - e * (bx - std::atan(bx))));
}

float PacejkaTire::load_scale(float load_n) const noexcept {
    const float positive_load = std::max(load_n, 0.0F);
    const float reference = std::max(coefficients_.reference_load_n, 1.0F);
    const float normalized = positive_load / reference;
    return std::max(0.0F, 1.0F + coefficients_.load_sensitivity * (normalized - 1.0F));
}

float PacejkaTire::longitudinal_curve(float slip, float load_n) const noexcept {
    const float scale = load_scale(load_n);
    return magic_formula(slip, coefficients_.longitudinal_b, coefficients_.longitudinal_c,
                         coefficients_.longitudinal_d * scale, coefficients_.longitudinal_e);
}

float PacejkaTire::lateral_curve(float slip_angle_rad, float load_n) const noexcept {
    const float scale = load_scale(load_n);
    return magic_formula(slip_angle_rad, coefficients_.lateral_b, coefficients_.lateral_c,
                         coefficients_.lateral_d * scale, coefficients_.lateral_e);
}

TireForce PacejkaTire::evaluate(const TireInput& input) const noexcept {
    const float surface = std::clamp(input.surface_friction, 0.0F, 2.0F);
    const float grip = std::clamp(input.tire_grip, 0.0F, 2.0F);
    const float load = std::max(input.normal_load_n, 0.0F);
    const float friction_scale = surface * grip * load;
    const float longitudinal_ratio = longitudinal_curve(input.longitudinal_slip, load);
    const float lateral_ratio = lateral_curve(input.slip_angle_rad, load);
    const float exponent = std::max(coefficients_.combined_exponent, 1.0F);
    const float demand = std::pow(std::pow(std::abs(longitudinal_ratio), exponent) +
                                      std::pow(std::abs(lateral_ratio), exponent),
                                  1.0F / exponent);
    const float utilization = demand > 1.0F ? 1.0F / demand : 1.0F;
    TireForce result;
    result.longitudinal_n = longitudinal_ratio * friction_scale * utilization;
    result.lateral_n = -lateral_ratio * friction_scale * utilization;
    result.utilization = std::clamp(demand, 0.0F, 1.0F);
    return result;
}

} // namespace indoru::vehicle
