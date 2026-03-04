cmake_minimum_required(VERSION 3.16)
project(locmgr_database VERSION 1.0.0 LANGUAGES CXX)

# ---------------------------------------------------------------------------
# C++ standard
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Compiler warnings
# ---------------------------------------------------------------------------
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Woverloaded-virtual
    -Wold-style-cast
)

# ---------------------------------------------------------------------------
# Library: locmgr_db
# ---------------------------------------------------------------------------
add_library(locmgr_db STATIC
    src/LocationDatabase.cpp
)

target_include_directories(locmgr_db
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Threads required for std::shared_mutex, std::atomic
find_package(Threads REQUIRED)
target_link_libraries(locmgr_db PUBLIC Threads::Threads)

# ---------------------------------------------------------------------------
# Demo executable
# ---------------------------------------------------------------------------
add_executable(locmgr_db_demo
    demo/main.cpp
)

target_link_libraries(locmgr_db_demo PRIVATE locmgr_db)

# ---------------------------------------------------------------------------
# Optional: install rules (useful when embedding in larger CMake project)
# ---------------------------------------------------------------------------
include(GNUInstallDirs)

install(TARGETS locmgr_db
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
