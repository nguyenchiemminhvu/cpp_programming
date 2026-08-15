cmake_minimum_required(VERSION 3.28)

# ── C++20 › modules ───────────────────────────────────────────────────────────
# Requires CMake ≥ 3.28 for FILE_SET CXX_MODULES support.
if(CMAKE_VERSION VERSION_LESS "3.28")
    message(WARNING "cpp20_modules skipped: CMake ≥ 3.28 required (found ${CMAKE_VERSION})")
    return()
endif()

# Module interface unit: automotive.sensor
add_library(automotive_sensor)
target_sources(automotive_sensor
    PUBLIC
        FILE_SET CXX_MODULES FILES sensor.cppm
)
set_target_properties(automotive_sensor PROPERTIES
    CXX_STANDARD          20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS        OFF
)

# Consumer executable
add_executable(cpp20_modules main.cpp)
set_target_properties(cpp20_modules PROPERTIES
    CXX_STANDARD          20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS        OFF
)
target_link_libraries(cpp20_modules PRIVATE automotive_sensor)
