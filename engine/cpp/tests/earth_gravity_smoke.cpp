#include "indoru/earth_gravity.hpp"
#include <cassert>
#include <cmath>
#include <limits>

int main() {
    using namespace indoru::earth;
    assert(valid_inputs(0.0, 0.0));
    assert(valid_inputs(90.0, 100'000.0));
    assert(!valid_inputs(91.0, 0.0));
    assert(!valid_inputs(0.0, 100'001.0));
    assert(!valid_inputs(0.0, std::numeric_limits<double>::quiet_NaN()));

    const double equator = normal_gravity(0.0);
    const double pole = normal_gravity(90.0);
    assert(equator > 9.7 && equator < 9.8);
    assert(pole > equator);
    assert(normal_gravity(45.0, 10'000.0) < normal_gravity(45.0, 0.0));

    const auto direction = sample(0.0, 90.0);
    assert(std::abs(direction.accelerationMetersPerSecondSquared - equator) < 1e-12);
    assert(std::abs(direction.downX) < 1e-12);
    assert(std::abs(direction.downY) < 1e-12);
    assert(std::abs(direction.downZ - 1.0) < 1e-12);
    assert(sample(0.0, 0.0, 0.0).accelerationMetersPerSecondSquared > 0.0);
    assert(sample(0.0, 0.0, 200'000.0).accelerationMetersPerSecondSquared == 0.0);
    assert(sample(0.0, std::numeric_limits<double>::quiet_NaN()).accelerationMetersPerSecondSquared == 0.0);
    assert(normal_gravity(std::numeric_limits<double>::quiet_NaN()) == 0.0);
    return 0;
}
