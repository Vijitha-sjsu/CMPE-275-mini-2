cmake_minimum_required(VERSION 3.10)

# Set some basic project attributes
project(mini2
    VERSION 0.1
    DESCRIPTION "A Hello World Project")
    
# Find the OpenMP package
find_package(OpenMP REQUIRED)

# Include the configuration header in the build and add CSVProcessor.cpp to the executable
add_executable(${PROJECT_NAME} mini2.cpp CSVProcessor.cpp)

# If OpenMP was found, link it to the target
if(OpenMP_CXX_FOUND OR OpenMP_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
endif()

# Create a simple configuration header
configure_file(config.h.in config.h)

# Include the configuration header in the build
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Explicitly setting OpenMP flags and libraries might not be necessary 
# if find_package(OpenMP REQUIRED) successfully finds and configures OpenMP
