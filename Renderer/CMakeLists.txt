# Vulkan Renderer Project File
# Ewan Burnett - 2024

project("VkRenderer")

set(CMAKE_CXX_STANDARD 17) 

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
set(EASY_PROFILER_NO_GUI ON CACHE BOOL "")
set(EASY_PROFILER_NO_SAMPLES ON CACHE BOOL "")

FetchContent_Declare(
	enkits
	GIT_REPOSITORY https://github.com/dougbinks/enkiTS
	GIT_TAG v1.11
)
# Disable EnkiTS Examples
set(ENKITS_BUILD_EXAMPLES OFF CACHE BOOL "")

FetchContent_Declare(
	vma
	GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
	GIT_TAG v3.0.1
)
#Disable VulkanMemoryAllocator samples
set(VMA_BUILD_SAMPLES OFF)

FetchContent_MakeAvailable(glfw imgui assimp stb easyprofiler enkits vma)

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

set(VKRENDERER_DEFINITIONS
	ENABLE_PROFILING=1
)

if(WIN32)
	set(VKRENDERER_DEFINITIONS ${VKRENDERER_DEFINITIONS} VKR_WIN32)
endif()

# These macros will be defined in Debug builds. 
set(VKRENDERER_DEBUG_DEFINITIONS 
    DEBUG 
    _DEBUG
)

add_library("VkRenderer" STATIC 
   "include/VKR.h"
   "src/VKR.cpp"
   "include/Types.h" 
   "include/Window.h" 
   "src/Window.cpp" 
   "include/Logger.h" 
   "src/Logger.cpp" 
   "include/Timer.h" 
   "src/Timer.cpp"
 "include/Random.h" "src/Random.cpp" "include/Maths.h" "include/Maths/Vector2.h" "include/Maths/Vector3.h" "include/Maths/Vector4.h" "include/Maths/Matrix.h" "include/Vulkan/VkCommon.h" "src/Vulkan/VkCommon.cpp" "include/Vulkan/VkContext.h" "src/Vulkan/VkContext.cpp" "include/Vulkan/VkInit.h" "src/Vulkan/VkInit.cpp" "include/Vulkan/VkHelpers.h" "src/Vulkan/VkHelpers.cpp" "include/Vulkan/VkSwapchain.h" "src/Vulkan/VkSwapchain.cpp")

# Link our dependencies
target_link_libraries("VkRenderer" PUBLIC Vulkan::Vulkan Threads::Threads glfw imgui easy_profiler enkiTS PRIVATE assimp VulkanMemoryAllocator)
target_include_directories("VkRenderer" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" PRIVATE ${glfw_SOURCE_DIR} ${stb_SOURCE_DIR})

target_compile_definitions("VkRenderer" PUBLIC ${VKRENDERER_DEFINITIONS})
add_compile_definitions("$<$<CONFIG:Debug>:${VKRENDERER_DEBUG_DEFINITIONS}>")
