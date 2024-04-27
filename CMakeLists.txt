# Vulkan Renderer Top-Level Project File 
# Ewan Burnett - 2024

cmake_minimum_required(VERSION 3.14)

project("Vulkan-Renderer")

# First, build the VKR Library
add_subdirectory("${CMAKE_SOURCE_DIR}/VKR")

# Then, build the dependent projects
add_subdirectory("${CMAKE_SOURCE_DIR}/Projects")