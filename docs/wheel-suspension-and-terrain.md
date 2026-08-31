# Indoru Wheel Suspension and Terrain Collision

## Implemented scope

Indoru के C++ vehicle layer में अब four-wheel raycast collision और spring-damper suspension foundation मौजूद है। हर wheel अपने local anchor से नीचे की ओर ray भेजता है। Terrain collider hit होने पर contact point, surface normal, friction, suspension length और compression निकाली जाती है।

`WheelColliderSystem` grounded wheels की संख्या, total normal force और average contact normal लौटाता है। `VehicleState::grounded` active wheel contacts के आधार पर update होता है। Airborne state में wheels ungrounded हो जाते हैं और normal force zero हो जाती है।

## Wheel model

| Component | Purpose |
|---|---|
| `Ray` | wheel anchor से terrain query |
| `RaycastHit` | contact, distance, normal और friction |
| `TerrainCollider` | engine physics backend के लिए abstract query interface |
| `HeightfieldTerrain` | flat-ground smoke-test और शुरुआती prototype implementation |
| `WheelTuning` | radius, travel, spring, damper, grip और driven/steering flags |
| `WheelState` | compression, contact, normal force और grounded state |
| `WheelRig` | four-wheel vehicle configuration |

Suspension force spring compression और compression velocity के आधार पर calculate होती है। Damper transient oscillation को कम करता है और normal force को negative होने से clamp किया जाता है। Ray length rest length, travel और wheel radius से बनती है। इससे wheel contact body height के अनुसार बदलता है।

## Current limits

यह अभी terrain query और wheel suspension layer है। Production vehicle backend के लिए अगले चरण में sloped mesh/BVH raycasts, wheel lateral/longitudinal tire forces, body torque, chassis collision shape, anti-roll bars, suspension force application, curb/step handling और network replication जोड़े जाएँगे। `HeightfieldTerrain` को future physics engine adapter से replace किया जा सकता है क्योंकि game code केवल `TerrainCollider` interface पर निर्भर है।

## Verification

C++ CTest में तीन smoke tests pass हुए हैं: existing world state, existing vehicle physics और new wheel collision. New wheel test flat terrain पर चार contacts, positive settled spring force, surface normal/friction, ray distance और airborne detection verify करता है।

## Files

- `engine/cpp/include/indoru/wheel_collision.hpp`
- `engine/cpp/src/wheel_collision.cpp`
- `engine/cpp/tests/wheel_collision_smoke.cpp`
- `engine/cpp/CMakeLists.txt`
