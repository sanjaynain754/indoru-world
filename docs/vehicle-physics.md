# Indoru Basic Vehicle Physics and Car Controller

## Scope

यह पहला C++ implementation एक lightweight arcade-physics foundation है। इसका उद्देश्य पहले playable car slice को स्थिर बनाना है, न कि production-grade tire simulator को replace करना। बाद में यही interfaces bikes, buses, trucks और boats के लिए specialized models में extend किए जा सकते हैं।

## Model

`VehicleState` position, velocity, yaw, angular velocity और grounded state रखता है। `VehicleInput` throttle, brake, steering और handbrake को normalized values में रखता है। `VehicleTuning` mass, engine force, brake force, rolling resistance, aerodynamic drag, steering angle, wheelbase, lateral grip और speed cap को data-driven parameters बनाता है।

हर physics step में input clamp होता है, forward और lateral velocity निकाली जाती है, drive/brake/rolling/drag forces calculate होती हैं और acceleration से velocity update होती है। Lateral grip sideways slip कम करती है। Handbrake grip घटाकर controlled slide की संभावना देता है। Bicycle-model style yaw calculation steering angle, wheelbase और forward speed पर आधारित है। Delta time को maximum step से clamp किया गया है ताकि frame spike से simulation explode न हो।

## Controller

`CarController` digital commands को smoothed analog input में बदलता है। Acceleration, reverse, brake और left/right steering commands धीरे-धीरे target values तक पहुँचते हैं, जिससे keyboard input abrupt नहीं लगता। Controller physics layer से अलग है; भविष्य में gamepad, touch और AI driver अलग input providers के रूप में जोड़े जा सकते हैं।

## Current limitations

यह implementation अभी ground collision, slopes, suspension springs, individual wheels, tire temperature, damage, gearbox, differential, traction-control और network prediction implement नहीं करता। Collision/world physics integration के लिए अगला layer vehicle body और physics backend को जोड़ेगा। Current `grounded` flag उस integration के लिए placeholder contract है।

## Extension policy

नई vehicle classes को `VehicleInput` और `VehicleState` के stable contract का उपयोग करना चाहिए। Shared math C++ core में रहेगा। Vehicle-specific tuning data JSON/schema से load किया जाएगा। Physics parameters को hardcode करके gameplay code में scatter नहीं किया जाएगा। हर नई model के लिए acceleration, braking, steering, reverse, max-speed और invalid-delta-time tests अनिवार्य होंगे।

## Files

- `engine/cpp/include/indoru/vehicle.hpp`
- `engine/cpp/src/vehicle.cpp`
- `engine/cpp/include/indoru/car_controller.hpp`
- `engine/cpp/src/car_controller.cpp`
- `engine/cpp/tests/vehicle_physics_smoke.cpp`
