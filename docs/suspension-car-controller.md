# Raycast Suspension Car Controller

## Overview

`SuspensionCarController` अब raycast wheel contacts को propulsion, braking और steering forces के साथ integrate करता है। हर simulation step पहले four-wheel suspension solve करता है, फिर grounded wheels पर tire forces calculate करता है और total chassis force तथा yaw torque से vehicle state update करता है।

## Force model

| Force | Application |
|---|---|
| Drive force | केवल `WheelTuning::driven` wheels पर throttle के अनुसार वितरित |
| Brake force | चारों grounded wheels में distribute; reverse motion में direction बदलती है |
| Lateral grip | wheel contact normal force, terrain friction और tire grip से clamp |
| Rolling resistance | grounded vehicle की motion direction में opposing force |
| Steering | front wheels के local forward/right axes को steering angle से rotate करता है |
| Yaw response | wheel force और local suspension arm के cross-product से torque |

Steering angle front wheels पर लागू होता है। Rear wheels vehicle heading के साथ aligned रहते हैं। Grounded wheel की normal force जितनी अधिक होगी, lateral tire force की उपलब्ध सीमा उतनी अधिक होगी। इससे airborne या low-contact vehicle को artificial full grip नहीं मिलेगा। Handbrake lateral grip को कम करता है और controlled slide का आधार देता है।

## Update order

प्रत्येक frame में controller input clamp करता है, `WheelColliderSystem::solve` से terrain contacts लेता है, grounded wheels पर wheel forces निकालता है, total force को mass से divide करके velocity update करता है और torque से yaw update करता है। Delta time 50 ms पर clamp रहता है ताकि frame spikes simulation को destabilize न करें।

## Current limits

यह एक stable prototype controller है। अभी body roll/pitch, full 3D angular inertia, suspension force का vertical chassis application, tire slip curves, differential, anti-roll bars, wheel collision meshes और network prediction शामिल नहीं हैं। `TerrainCollider` abstraction को future mesh/BVH physics backend से जोड़ा जा सकता है।

## Files

- `engine/cpp/include/indoru/suspension_car_controller.hpp`
- `engine/cpp/src/suspension_car_controller.cpp`
- `engine/cpp/tests/suspension_car_controller_smoke.cpp`
- `engine/cpp/CMakeLists.txt`

## Verification

C++ CTest में world-state, basic vehicle, wheel collision और integrated suspension-car controller के चारों smoke tests pass होने चाहिए। Controller test acceleration से speed बढ़ना, braking से speed घटना, grounded contact बने रहना और steering से yaw बदलना verify करता है।
