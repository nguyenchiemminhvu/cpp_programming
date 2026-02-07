cmake_minimum_required(VERSION 3.10)
project(CppOptimization)

add_executable(main main.cpp)

target_include_directories(main
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(main PRIVATE timer.cpp performancetest.cpp)

add_library(perftimer SHARED timer.cpp performancetest.cpp)

target_include_directories(perftimer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(
    TARGETS perftimer
    EXPORT PerfTimerTarget
    LIBRARY DESTINATION lib
    INCLUDES DESTINATION include
)

install(
    EXPORT PerfTimerTarget
    FILE PerfTimerTarget.cmake
    NAMESPACE PerfTimerLib
    DESTINATION lib/cmake/PerfTimer
)

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/timer.h ${CMAKE_CURRENT_SOURCE_DIR}/performancetest.h
    DESTINATION include
)

include(CMakePackageConfigHelpers)

configure_file(PerfTimerConfig.cmake.in PerfTimerConfig.cmake @ONLY)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/PerfTimerConfig.cmake"
    DESTINATION lib/cmake/PerfTimer
)
