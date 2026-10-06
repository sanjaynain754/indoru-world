cmake_minimum_required(VERSION 3.24)
project(indoru_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(indoru_engine STATIC
    src/engine.cpp
    src/world_state.cpp
    src/avarra_region_content.cpp
    src/vehicle.cpp
    src/car_controller.cpp
    src/wheel_collision.cpp
    src/suspension_car_controller.cpp
    src/tire_model.cpp
    src/city_building_framework.cpp
    src/residential_building_placement.cpp
    src/residential_simulation.cpp
    src/world_simulation_bridge.cpp
    src/state_system.cpp
)

target_include_directories(indoru_engine PUBLIC include)
target_compile_options(indoru_engine PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)

add_executable(indoru_smoke_test tests/world_state_smoke.cpp)
target_link_libraries(indoru_smoke_test PRIVATE indoru_engine)
target_compile_options(indoru_smoke_test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)

add_executable(indoru_vehicle_smoke_test tests/vehicle_physics_smoke.cpp)
target_link_libraries(indoru_vehicle_smoke_test PRIVATE indoru_engine)
target_compile_options(indoru_vehicle_smoke_test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)

enable_testing()
add_test(NAME world_state_smoke COMMAND indoru_smoke_test)
add_test(NAME vehicle_physics_smoke COMMAND indoru_vehicle_smoke_test)

add_executable(indoru_wheel_collision_test tests/wheel_collision_smoke.cpp)
target_link_libraries(indoru_wheel_collision_test PRIVATE indoru_engine)
target_compile_options(indoru_wheel_collision_test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
add_test(NAME wheel_collision_smoke COMMAND indoru_wheel_collision_test)

add_executable(indoru_suspension_car_test tests/suspension_car_controller_smoke.cpp)
target_link_libraries(indoru_suspension_car_test PRIVATE indoru_engine)
target_compile_options(indoru_suspension_car_test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
add_test(NAME suspension_car_controller_smoke COMMAND indoru_suspension_car_test)

add_executable(indoru_avarra_region_content_test tests/avarra_region_content_smoke.cpp)
target_link_libraries(indoru_avarra_region_content_test PRIVATE indoru_engine)
target_compile_options(indoru_avarra_region_content_test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
add_test(NAME avarra_region_content_smoke COMMAND indoru_avarra_region_content_test)

add_executable(indoru_gravity_suspension_test tests/gravity_suspension_smoke.cpp)
target_link_libraries(indoru_gravity_suspension_test PRIVATE indoru_engine)
target_compile_options(indoru_gravity_suspension_test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
add_test(NAME gravity_suspension_smoke COMMAND indoru_gravity_suspension_test)

add_executable(indoru_tire_model_test tests/tire_model_smoke.cpp)
target_link_libraries(indoru_tire_model_test PRIVATE indoru_engine)
target_compile_options(indoru_tire_model_test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
add_test(NAME tire_model_smoke COMMAND indoru_tire_model_test)

add_executable(indoru_city_building_framework_test tests/p1_city_building_framework_smoke.cpp)
target_link_libraries(indoru_city_building_framework_test PRIVATE indoru_engine)
target_compile_options(indoru_city_building_framework_test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
add_test(NAME city_building_framework_smoke COMMAND indoru_city_building_framework_test)

add_executable(indoru_residential_building_placement_test tests/p1_residential_building_placement_smoke.cpp)
target_link_libraries(indoru_residential_building_placement_test PRIVATE indoru_engine)
target_compile_options(indoru_residential_building_placement_test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
add_test(NAME residential_building_placement_smoke COMMAND indoru_residential_building_placement_test)

add_executable(indoru_residential_household_test tests/p1_residential_household_smoke.cpp)
target_link_libraries(indoru_residential_household_test PRIVATE indoru_engine)
target_compile_options(indoru_residential_household_test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
add_test(NAME residential_household_smoke COMMAND indoru_residential_household_test)

add_executable(indoru_world_simulation_bridge_test tests/world_simulation_bridge_smoke.cpp)
target_link_libraries(indoru_world_simulation_bridge_test PRIVATE indoru_engine)
target_compile_options(indoru_world_simulation_bridge_test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
add_test(NAME world_simulation_bridge_smoke COMMAND indoru_world_simulation_bridge_test)

add_executable(indoru_state_system_test tests/state_system_smoke.cpp)
target_link_libraries(indoru_state_system_test PRIVATE indoru_engine)
target_compile_options(indoru_state_system_test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
add_test(NAME state_system_smoke COMMAND indoru_state_system_test)
