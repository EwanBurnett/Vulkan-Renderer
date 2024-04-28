project("01-Hello-Triangle")

set(CMAKE_CXX_STANDARD 17) 

add_executable(${PROJECT_NAME} "01-Hello-Triangle.cpp")
target_link_libraries(${PROJECT_NAME} PUBLIC VKR Shared)
add_compile_definitions("$<$<CONFIG:Debug>:DEBUG>")

# Copy application resources to the binary directory on build
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Data/
        ${CMAKE_CURRENT_BINARY_DIR}/Data/
)
