project("01-Hello-Triangle")

set(CMAKE_CXX_STANDARD 17) 

set(PROJECT_SOURCES "main.cpp" "01-Hello-Triangle.cpp""01-Hello-Triangle.h")

# Build Resource file in windows builds. 
if(WIN32)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} "Resources.rc")
endif()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC VKR Shared)
add_compile_definitions("$<$<CONFIG:Debug>:DEBUG>")

# Copy application resources to the binary directory on build
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Data/
        ${CMAKE_CURRENT_BINARY_DIR}/Data/
)
