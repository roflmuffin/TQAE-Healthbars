cmake_minimum_required(VERSION 3.31)
project(Ormenos)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_GENERATOR_PLATFORM Win32)

include(FetchContent)

add_library(Ormenos SHARED
    src/main.cpp
    src/dinput8_proxy.cpp
    src/d3d11_impl.cpp
    src/health_bar_config.cpp
    src/shared.cpp
)

# Build as dinput8.dll so it can be dropped into the game folder and load via Windows DLL search order.
set_target_properties(Ormenos PROPERTIES
    OUTPUT_NAME "dinput8"
    PREFIX ""
)

add_subdirectory(vendor)

target_link_libraries(Ormenos PRIVATE minhook)
