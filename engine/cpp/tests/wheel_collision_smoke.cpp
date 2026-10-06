#include "indoru/wheel_collision.hpp"

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

    const indoru::vehicle::HeightfieldTerrain ground(0.0F, 0.95F);
    const indoru::vehicle::WheelColliderSystem colliders;
    auto grounded = colliders.solve(vehicle, rig, ground, 1.0F / 60.0F);
    for (int i = 0; i < 8; ++i) {
        grounded = colliders.solve(vehicle, rig, ground, 1.0F / 60.0F);
    }

    // The first contact must not inject a stale compression-delta impulse.
    assert(grounded.grounded_wheels == 4);
    assert(vehicle.grounded);
    assert(grounded.total_normal_force_n > 0.0F);
    for (const auto& wheel : rig.wheels) {
        assert(wheel.grounded);
        assert(wheel.compression >= 0.0F && wheel.compression <= 1.0F);
        assert(std::abs(wheel.contact.normal.y - 1.0F) < 0.001F);
        assert(wheel.contact.friction > 0.0F);
    }

    vehicle.position.y = 10.0F;
    const auto airborne = colliders.solve(vehicle, rig, ground, 1.0F / 60.0F);
    assert(airborne.grounded_wheels == 0);
    assert(!vehicle.grounded);

    const indoru::vehicle::Ray ray{{0.0F, 2.0F, 0.0F}, {0.0F, -1.0F, 0.0F}, 3.0F};
    const auto hit = ground.raycast(ray);
    assert(hit.hit);
    assert(std::abs(hit.distance - 2.0F) < 0.001F);

    const indoru::vehicle::HeightfieldTerrain slope({0.0F, 0.5F, 1.0F,
                                                      0.0F, 0.5F, 1.0F,
                                                      0.0F, 0.5F, 1.0F},
                                                     3, 3, 1.0F, -1.0F, -1.0F, 0.8F);
    const auto slope_hit = slope.raycast({{0.0F, 3.0F, 0.0F}, {0.0F, -1.0F, 0.0F}, 4.0F});
    assert(slope_hit.hit);
    assert(std::abs(slope_hit.point.y - 0.5F) < 0.001F);
    assert(slope_hit.normal.y > 0.85F && slope_hit.normal.y < 1.0F);
    assert(std::abs(slope_hit.normal.x) > 0.2F);

    std::cout << "Indoru wheel collision smoke test passed\n";
    return 0;
}
