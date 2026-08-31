#include "indoru/suspension_car_controller.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    indoru::vehicle::VehicleState vehicle{};
    vehicle.position = {0.0F, 0.8F, 0.0F};

    indoru::vehicle::WheelRig rig{};
    rig.wheels[0].position = indoru::vehicle::WheelPosition::FrontLeft;
    rig.wheels[1].position = indoru::vehicle::WheelPosition::FrontRight;
    rig.wheels[2].position = indoru::vehicle::WheelPosition::RearLeft;
    rig.wheels[3].position = indoru::vehicle::WheelPosition::RearRight;
    rig.wheels[0].local_anchor = {-0.85F, 0.0F, 1.2F};
    rig.wheels[1].local_anchor = {0.85F, 0.0F, 1.2F};
    rig.wheels[2].local_anchor = {-0.85F, 0.0F, -1.2F};
    rig.wheels[3].local_anchor = {0.85F, 0.0F, -1.2F};

    const indoru::vehicle::HeightfieldTerrain terrain(0.0F, 1.0F);
    const indoru::vehicle::SuspensionCarController controller;
    indoru::vehicle::SuspensionCarInput input{};

    for (int i = 0; i < 10; ++i) {
        controller.update(vehicle, rig, input, terrain, 1.0F / 60.0F);
    }
    input.throttle = 1.0F;
    for (int i = 0; i < 120; ++i) {
        controller.update(vehicle, rig, input, terrain, 1.0F / 60.0F);
    }
    const float accelerated_speed = std::sqrt(vehicle.velocity.x * vehicle.velocity.x + vehicle.velocity.z * vehicle.velocity.z);
    assert(vehicle.grounded);
    assert(accelerated_speed > 1.0F);

    input.throttle = 0.0F;
    input.brake = 1.0F;
    for (int i = 0; i < 60; ++i) {
        controller.update(vehicle, rig, input, terrain, 1.0F / 60.0F);
    }
    const float braked_speed = std::sqrt(vehicle.velocity.x * vehicle.velocity.x + vehicle.velocity.z * vehicle.velocity.z);
    assert(braked_speed < accelerated_speed);

    input.brake = 0.0F;
    input.throttle = 1.0F;
    input.steering = 1.0F;
    const float yaw_before = vehicle.yaw_rad;
    for (int i = 0; i < 60; ++i) {
        controller.update(vehicle, rig, input, terrain, 1.0F / 60.0F);
    }
    assert(std::abs(vehicle.yaw_rad - yaw_before) > 0.001F);
    assert(std::isfinite(vehicle.roll_rad));
    assert(std::isfinite(vehicle.pitch_rad));
    assert(std::abs(vehicle.roll_rad) <= 0.35F + 0.001F);
    assert(std::abs(vehicle.pitch_rad) <= 0.25F + 0.001F);

    std::cout << "Indoru suspension car controller smoke test passed\n";
    return 0;
}
