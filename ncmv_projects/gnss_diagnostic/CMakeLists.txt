cmake_minimum_required(VERSION 3.16)
project(gnss_diagnostic CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(gnss_diagnostic STATIC
    src/dtc_monitor_base.cpp
    src/pvt_timeout_monitor.cpp
    src/antenna_short_monitor.cpp
    src/antenna_open_monitor.cpp
    src/recovery_manager.cpp
    src/chip_reset_action.cpp
    src/platform_reboot_action.cpp
    src/power_mode_controller.cpp
    src/gnss_diagnostic_manager.cpp
)

target_include_directories(gnss_diagnostic
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(gnss_diagnostic PRIVATE
    -Wall -Wextra -Wpedantic
)

# Tests are wired separately under test/ when added.
