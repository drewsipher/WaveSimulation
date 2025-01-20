cmake_minimum_required(VERSION 3.10)

# Project name
project(AudioWaveSim)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenCV package
set(OpenCV_DIR "C:/opencv/build")  # Update this path to your OpenCV build directory
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(src)

# Source files
set(SOURCES
    main.cpp)
#     src/SimulationGrid.cpp
#     src/WaveSimulation.cpp
#     src/Visualizer.cpp
# )

# Add executable
add_executable(AudioWaveSim ${SOURCES})

# Link OpenCV libraries
target_link_libraries(AudioWaveSim ${OpenCV_LIBS})