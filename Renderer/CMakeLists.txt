# Vulkan Renderer Project File
# Ewan Burnett - 2024

project("VkRenderer")

find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)

# Fetch Dependencies 
include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
)

FetchContent_Declare(
    imgui   
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
)

FetchContent_Declare(
	assimp
	GIT_REPOSITORY https://github.com/assimp/assimp
)

FetchContent_Declare(
	stb
	GIT_REPOSITORY https://github.com/nothings/stb 
)

FetchContent_Declare(
	easyprofiler
	GIT_REPOSITORY https://github.com/yse/easy_profiler 
	GIT_TAG v2.1.0
)
# Disable EasyProfiler GUI and Sample builds by default
#set(EASY_PROFILER_NO_GUI ON CACHE BOOL "")
set(EASY_PROFILER_NO_SAMPLES ON CACHE BOOL "")

FetchContent_MakeAvailable(glfw imgui assimp stb easyprofiler)

# Manually Build ImGui
add_library(imgui 
	"${imgui_SOURCE_DIR}/imgui.cpp" 
	"${imgui_SOURCE_DIR}/imgui_demo.cpp"
	"${imgui_SOURCE_DIR}/imgui_draw.cpp" 
	"${imgui_SOURCE_DIR}/imgui_tables.cpp" 
	"${imgui_SOURCE_DIR}/imgui_widgets.cpp" 
	"${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
)

target_link_libraries(imgui glfw)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

# These macros will be defined in Debug builds. 
set(VKRENDERER_DEBUG_DEFINITIONS 
    DEBUG 
    _DEBUG
	ENABLE_PROFILING
)

add_library("VkRenderer" STATIC 
   "include/Renderer.h"
   "src/Renderer.cpp"
 "include/Types.h" "include/Window.h" "src/Window.cpp" "include/Logger.h" "src/Logger.cpp" "include/Timer.h" "src/Timer.cpp")

# Link our dependencies
target_link_libraries("VkRenderer" Vulkan::Vulkan Threads::Threads glfw imgui assimp easy_profiler)
target_include_directories("VkRenderer" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" PRIVATE ${glfw_SOURCE_DIR} ${stb_SOURCE_DIR})

target_compile_definitions("VkRenderer" PRIVATE $<$<CONFIG:Debug>:${VKRENDERER_DEBUG_DEFINITIONS}>)
