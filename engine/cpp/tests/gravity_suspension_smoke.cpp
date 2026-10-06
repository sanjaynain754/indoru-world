#include "indoru/suspension_car_controller.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    indoru::vehicle::WheelRig rig{};
    rig.wheels[0].position = indoru::vehicle::WheelPosition::FrontLeft;
    rig.wheels[1].position = indoru::vehicle::WheelPosition::FrontRight;
    rig.wheels[2].position = indoru::vehicle::WheelPosition::RearLeft;
    rig.wheels[3].position = indoru::vehicle::WheelPosition::RearRight;
    rig.wheels[0].local_anchor = {-0.85F, 0.0F, 1.2F};
    rig.wheels[1].local_anchor = {0.85F, 0.0F, 1.2F};
    rig.wheels[2].local_anchor = {-0.85F, 0.0F, -1.2F};
    rig.wheels[3].local_anchor = {0.85F, 0.0F, -1.2F};

    const indoru::vehicle::HeightfieldTerrain ground(0.0F, 1.0F);
    const indoru::vehicle::SuspensionCarController controller;
    indoru::vehicle::VehicleState grounded_vehicle{};
    grounded_vehicle.position = {0.0F, 0.8F, 0.0F};
    const auto grounded = controller.update(grounded_vehicle, rig, {}, ground, 1.0F / 60.0F);

    assert(grounded.suspension.grounded_wheels == 4);
    assert(grounded.suspension.total_contact_force.y > 0.0F);
    assert(grounded.total_force.y > -1500.0F * 9.81F);
    assert(std::isfinite(grounded.suspension.total_contact_force.y));

    indoru::vehicle::VehicleState airborne_vehicle{};
    airborne_vehicle.position = {0.0F, 10.0F, 0.0F};
    const auto airborne = controller.update(airborne_vehicle, rig, {}, ground, 1.0F / 60.0F);
    assert(airborne.suspension.grounded_wheels == 0);
    assert(airborne.suspension.total_contact_force.y == 0.0F);
    assert(std::abs(airborne.total_force.y + 1500.0F * 9.81F) < 0.01F);
    assert(airborne_vehicle.velocity.y < 0.0F);

    std::cout << "Indoru gravity and suspension force smoke test passed\n";
    return 0;
}
