cmake_minimum_required(VERSION 3.31)
project(Ormenos)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_GENERATOR_PLATFORM Win32)

if(MSVC)
    # Titan Quest ships the VS2015 runtime, which must initialize its own mutex storage.
    add_compile_definitions(_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR)
endif()

include(FetchContent)

add_library(Ormenos SHARED
    src/main.cpp
    src/dinput8_proxy.cpp
    src/d3d11_impl.cpp
    src/health_bar_config.cpp
    src/hit_flash.cpp
    src/damage_numbers.cpp
    src/gold_autoloot.cpp
    src/loot_notifications.cpp
    src/inventory_glow.cpp
    src/shared.cpp
)

# Build as dinput8.dll so it can be dropped into the game folder and load via Windows DLL search order.
set_target_properties(Ormenos PROPERTIES
    OUTPUT_NAME "dinput8"
    PREFIX ""
)

add_subdirectory(vendor)

target_link_libraries(Ormenos PRIVATE minhook)

add_executable(hit_flash_shader_test EXCLUDE_FROM_ALL tests/hit_flash_shader_test.cpp)
target_link_libraries(hit_flash_shader_test PRIVATE d3d11 d3dcompiler)

add_executable(damage_numbers_test EXCLUDE_FROM_ALL tests/damage_numbers_test.cpp)

add_executable(gold_autoloot_test EXCLUDE_FROM_ALL tests/gold_autoloot_test.cpp src/health_bar_config.cpp src/inventory_glow.cpp)
target_include_directories(gold_autoloot_test PRIVATE vendor/minhook/include)
target_link_libraries(gold_autoloot_test PRIVATE minhook)

add_executable(inventory_glow_test EXCLUDE_FROM_ALL tests/inventory_glow_test.cpp src/health_bar_config.cpp)
target_include_directories(inventory_glow_test PRIVATE vendor/minhook/include)
target_link_libraries(inventory_glow_test PRIVATE minhook)
