cmake_minimum_required(VERSION 3.10)

# Project name
project(AudioWaveSim)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set build type to Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add compiler warnings
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find OpenCV package
find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found!")
endif()

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(src)

# Source files
set(SOURCES
    main.cpp
    src/SimulationGrid.cpp
    src/Physics.cpp
    src/Visualizer.cpp
)

# Add executable
add_executable(AudioWaveSim ${SOURCES})

# Link OpenCV libraries
target_link_libraries(AudioWaveSim ${OpenCV_LIBS})