# Vehicle Body Roll, Pitch and Anti-Roll Bars

## Implemented behavior

`SuspensionCarController` अब wheel suspension contacts के आधार पर chassis orientation dynamics calculate करता है। `VehicleState` में persistent `roll_rad`, `pitch_rad`, `roll_velocity` और `pitch_velocity` fields हैं। Steering/cornering acceleration roll response पैदा करती है, acceleration और braking longitudinal pitch response पैदा करते हैं, और spring-damper terms motion को rest orientation की ओर वापस लाते हैं।

## Anti-roll bars

Front और rear axle पर compression difference से anti-roll force calculate होती है। यदि left और right wheel compression अलग है, तो bar heavier-compressed side को opposite normal force और दूसरी side को balancing force देता है। इससे cornering में body lean कम होती है, लेकिन suspension पूरी तरह rigid नहीं बनती। Front और rear anti-roll stiffness अलग tuning values हैं, इसलिए understeer/oversteer behavior पर आगे tuning की जा सकती है।

## Tuning parameters

| Parameter | Purpose |
|---|---|
| `roll_inertia_kg_m2` | roll angular response |
| `pitch_inertia_kg_m2` | pitch angular response |
| `roll_stiffness_n_m_per_rad` | body को neutral roll की ओर लौटाता है |
| `roll_damping_n_m_s_per_rad` | roll oscillation कम करता है |
| `pitch_stiffness_n_m_per_rad` | nose-up/down recovery |
| `pitch_damping_n_m_s_per_rad` | braking/acceleration bounce कम करता है |
| `front_anti_roll_n_per_compression` | front axle roll resistance |
| `rear_anti_roll_n_per_compression` | rear axle roll resistance |
| `max_roll_rad` | safety orientation limit |
| `max_pitch_rad` | safety orientation limit |

## Current limits

यह implementation gameplay-grade body-dynamics foundation है। अभी complete 3D quaternion orientation, chassis collision impulse, suspension vertical force का full rigid-body application, weight transfer through contact patches, anti-roll bar torque visualization और network rollback शामिल नहीं हैं। Future physics backend इन interfaces को full rigid-body solver से replace या extend कर सकता है।

## Verification

चारों C++ smoke tests pass हुए हैं। Existing world-state, basic vehicle, wheel collision और integrated suspension controller tests के साथ controller test finite roll/pitch values, orientation limits और steering response verify करता है।

## Files

- `engine/cpp/include/indoru/vehicle.hpp`
- `engine/cpp/include/indoru/suspension_car_controller.hpp`
- `engine/cpp/src/suspension_car_controller.cpp`
- `engine/cpp/tests/suspension_car_controller_smoke.cpp`
