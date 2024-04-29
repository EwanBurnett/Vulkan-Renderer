# Vulkan Renderer Top-Level Project File 
# Ewan Burnett - 2024

cmake_minimum_required(VERSION 3.14)

project("Vulkan-Renderer")

option(VKR_BUILD_SAMPLES "Build VKR Sample Applications" ON)

# First, build the VKR Library
add_subdirectory("${CMAKE_SOURCE_DIR}/VKR")

# Then, build the dependent projects
if(${VKR_BUILD_SAMPLES})
    message(STATUS "Building VKR Sample Projects.")
    add_subdirectory("${CMAKE_SOURCE_DIR}/Samples")
endif()