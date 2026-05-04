cmake_minimum_required(VERSION 3.31)
project(KieroHookTest)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_GENERATOR_PLATFORM Win32)

include(FetchContent)


add_library(KieroHookTest SHARED
    main.cpp
    dinput8_proxy.cpp
    impl/d3d11_impl.cpp
    impl/shared.cpp
    impl/win32_impl.cpp
)

# Build as dinput8.dll so it can be dropped into the game folder and load via Windows DLL search order.
set_target_properties(KieroHookTest PROPERTIES
    OUTPUT_NAME "dinput8"
    PREFIX ""
)

add_subdirectory(vendor)

target_link_libraries(KieroHookTest PRIVATE minhook)
