# Pacejka-Style Tire Slip and Combined Friction

## Scope

Indoru vehicle controller में अब Pacejka-style magic formula tire model जोड़ा गया है। यह full laboratory-calibrated tire model नहीं है; coefficients game tuning के लिए data-driven starting point हैं। इसका उद्देश्य acceleration, braking और cornering को एक ही wheel-force pipeline में जोड़ना है।

## Slip inputs

Longitudinal slip driven और braking wheels की rotational/forward-speed difference से निकाला जाता है। Positive slip drive demand और negative slip braking demand को represent करता है। Lateral slip angle wheel heading और contact-point velocity के बीच angle से calculate होता है। Low-speed divide-by-zero और unstable values से बचने के लिए contact-speed floor लागू है।

## Pacejka-style curves

Longitudinal और lateral force ratios magic formula के `B`, `C`, `D`, `E` coefficients से calculate होते हैं। Normal load के आधार पर load scaling लागू होती है, फिर surface friction और tire grip multiply होकर available force बनती है। यह setup अलग surfaces—जैसे dry road, wet road, sand या ice—के लिए `surface_friction` बदलने देता है।

## Combined friction

एक wheel एक साथ braking/acceleration और cornering कर सकता है। इसलिए longitudinal और lateral demands को power-norm friction ellipse में combine किया जाता है। यदि combined demand limit से अधिक हो, तो दोनों forces proportionally scale होती हैं। इससे tire unlimited longitudinal और lateral grip एक साथ नहीं दे सकता। `TireForce::utilization` combined demand की normalized value report करता है।

## Controller integration

`SuspensionCarController` अब प्रत्येक grounded wheel के लिए normal load, terrain friction, tire grip, longitudinal slip और slip angle `PacejkaTire::evaluate` को देता है। Returned longitudinal force propulsion/braking में और lateral force steering/grip में लगती है। Anti-roll forces, body roll/pitch और yaw torque इसी wheel-force result के साथ आगे भी लागू रहते हैं।

## Tuning parameters

| Parameter | अर्थ |
|---|---|
| `longitudinal_b/c/d/e` | acceleration और braking curve shape |
| `lateral_b/c/d/e` | cornering curve shape |
| `reference_load_n` | load-scaling reference |
| `load_sensitivity` | normal-load के साथ peak ratio बदलना |
| `combined_exponent` | friction ellipse का shape |
| `surface_friction` | terrain/material grip multiplier |
| `tire_grip` | vehicle tyre quality multiplier |

## Current limits

यह implementation wheel angular dynamics, true driven-wheel torque, tire relaxation length, camber thrust, temperature, wear, deformation और calibrated vehicle data implement नहीं करता। अगली tuning phase में wheel rotational state, differential, ABS/traction control और per-surface coefficient tables जोड़े जा सकते हैं।

## Verification

C++ CTest में पाँच tests pass हुए: world state, basic vehicle physics, wheel collision, integrated suspension controller और Pacejka tire model। Tire test neutral force, positive drive force, negative brake force, lateral cornering force, combined utilization और low-friction scaling verify करता है।

## Files

- `engine/cpp/include/indoru/tire_model.hpp`
- `engine/cpp/src/tire_model.cpp`
- `engine/cpp/tests/tire_model_smoke.cpp`
- `engine/cpp/include/indoru/suspension_car_controller.hpp`
- `engine/cpp/src/suspension_car_controller.cpp`
