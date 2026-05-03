cmake_minimum_required(VERSION 3.31)
project(KieroHookTest)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_GENERATOR_PLATFORM Win32)

include(FetchContent)


add_library(KieroHookTest SHARED main.cpp impl/d3d11_impl.cpp impl/shared.cpp impl/win32_impl.cpp)

add_subdirectory(vendor)

target_link_libraries(KieroHookTest PRIVATE minhook)

# DLL Injector (standalone executable, no dependencies beyond Win32)
add_executable(TQInjector injector/injector.cpp)
set_target_properties(TQInjector PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
