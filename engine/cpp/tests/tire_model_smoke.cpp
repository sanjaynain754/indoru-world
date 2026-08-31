#include "indoru/tire_model.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    const indoru::vehicle::PacejkaTire tire;
    const auto neutral = tire.evaluate({0.0F, 0.0F, 3500.0F, 1.0F, 1.0F});
    assert(std::abs(neutral.longitudinal_n) < 0.01F);
    assert(std::abs(neutral.lateral_n) < 0.01F);

    const auto drive = tire.evaluate({0.12F, 0.0F, 3500.0F, 1.0F, 1.0F});
    const auto brake = tire.evaluate({-0.12F, 0.0F, 3500.0F, 1.0F, 1.0F});
    assert(drive.longitudinal_n > 0.0F);
    assert(brake.longitudinal_n < 0.0F);

    const auto corner = tire.evaluate({0.0F, 0.12F, 3500.0F, 1.0F, 1.0F});
    assert(corner.lateral_n < 0.0F);

    const auto combined = tire.evaluate({0.18F, 0.18F, 3500.0F, 1.0F, 1.0F});
    assert(combined.utilization <= 1.0F);
    assert(std::isfinite(combined.longitudinal_n));
    assert(std::isfinite(combined.lateral_n));

    const auto low_friction = tire.evaluate({0.12F, 0.12F, 3500.0F, 0.35F, 1.0F});
    assert(std::abs(low_friction.longitudinal_n) < std::abs(combined.longitudinal_n));
    assert(std::abs(low_friction.lateral_n) < std::abs(combined.lateral_n));

    std::cout << "Indoru Pacejka tire model smoke test passed\n";
    return 0;
}
