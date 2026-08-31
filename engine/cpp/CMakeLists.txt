cmake_minimum_required(VERSION 3.24)
project(indoru_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(indoru_engine STATIC
    src/engine.cpp
    src/world_state.cpp
    src/vehicle.cpp
    src/car_controller.cpp
    src/wheel_collision.cpp
    src/suspension_car_controller.cpp
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
