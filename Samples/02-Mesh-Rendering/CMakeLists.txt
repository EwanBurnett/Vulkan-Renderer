project("02-Mesh-Rendering")

set(CMAKE_CXX_STANDARD 17) 

set(PROJECT_SOURCES "main.cpp" "Renderer/Scene.h" "Renderer/Scene.cpp" "Renderer/Camera.h" "Renderer/Camera.cpp" "Renderer/Importer.h" "Renderer/Importer.cpp" "Renderer/GUI.h" "Renderer/GUI.cpp" "Renderer/Renderer.h" "Renderer/Renderer.cpp")

# Build Resource file in windows builds. 
if(WIN32)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} "Resources.rc")
endif()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC VKR Shared)
add_compile_definitions("$<$<CONFIG:Debug>:DEBUG>")

# Compile Shaders before building 
add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/Data/Shaders/
)

# Retrieve a list of all shaders
file(GLOB_RECURSE SHADER_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*)

# Compile each shader 
function(CompileShaders SHADER_SRC_PATH)    
    cmake_path(GET SHADER_SRC_PATH FILENAME SHADER_DST_PATH)
    set(SHADER_DST_PATH ${CMAKE_CURRENT_BINARY_DIR}/Data/Shaders/${SHADER_DST_PATH}.spirv)

    message(STATUS "Compiling Shader ${SHADER_SRC_PATH} -> ${SHADER_DST_PATH}")
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER_SRC_PATH} -o ${SHADER_DST_PATH}
    )
endfunction()

foreach(shader ${SHADER_SOURCE_FILES})
    CompileShaders(${shader})
endforeach()

# Copy application resources to the binary directory on build
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Data/
        ${CMAKE_CURRENT_BINARY_DIR}/Data/
)
