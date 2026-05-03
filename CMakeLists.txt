cmake_minimum_required(VERSION 3.31)
project(KieroHookTest)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_GENERATOR_PLATFORM Win32)

include(FetchContent)

# Safetyhook
FetchContent_Declare(
        safetyhook
        GIT_REPOSITORY "https://github.com/cursey/safetyhook.git"
        GIT_TAG "origin/main"
)
FetchContent_MakeAvailable(safetyhook)

add_library(KieroHookTest SHARED main.cpp impl/d3d11_impl.cpp impl/shared.cpp impl/win32_impl.cpp)

add_subdirectory(vendor)

target_link_libraries(KieroHookTest PRIVATE minhook safetyhook)
