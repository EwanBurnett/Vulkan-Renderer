# VKR Project File
# Ewan Burnett - 2024

project("VKR" VERSION 0.0.18)

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

target_link_libraries(imgui glfw Vulkan::Vulkan)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

set(VKR_DEFINITIONS )

if(WIN32)
	set(VKR_DEFINITIONS ${VKR_DEFINITIONS} VKR_WIN32)
endif()

# These macros will be defined in Debug builds. 
set(VKR_DEBUG_DEFINITIONS 
	VKR_PROFILER_DUMP=0
	VKR_DEBUG=1
    DEBUG 
    _DEBUG
)

add_library("VKR" STATIC 
   "include/VKR/VKR.h"
   "src/VKR.cpp"
   "include/VKR/Types.h" 
   "include/VKR/Window.h" 
   "src/Window.cpp" 
   "include/VKR/Logger.h" 
   "src/Logger.cpp" 
   "include/VKR/Timer.h" 
   "include/VKR/File.h"
   "src/File.cpp"
   "src/Timer.cpp"
   "include/VKR/Random.h" 
   "src/Random.cpp" 
   "include/VKR/Maths.h"
   "include/VKR/Maths/Vector2.h"
   "include/VKR/Maths/Vector3.h" 
   "include/VKR/Maths/Vector4.h" 
   "include/VKR/Maths/Matrix.h" 
   "include/VKR/Vulkan/VkCommon.h" 
   "src/Vulkan/VkCommon.cpp" 
   "include/VKR/Vulkan/VkContext.h"
   "src/Vulkan/VkContext.cpp"
   "include/VKR/Vulkan/VkInit.h" 
   "src/Vulkan/VkInit.cpp"
   "include/VKR/Vulkan/VkHelpers.h"
   "src/Vulkan/VkHelpers.cpp"
   "include/VKR/Vulkan/VkSwapchain.h"
   "src/Vulkan/VkSwapchain.cpp"
   "include/VKR/Vulkan/VkPipelineBuilder.h"
   "src/Vulkan/VkPipelineBuilder.cpp"
)

# Link our dependencies
target_link_libraries("VKR" PUBLIC Vulkan::Vulkan Threads::Threads glfw imgui easy_profiler enkiTS assimp VulkanMemoryAllocator)
target_include_directories("VKR" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/" ${glfw_SOURCE_DIR} ${stb_SOURCE_DIR} ${imgui_SOURCE_DIR})

target_compile_definitions("VKR" PUBLIC ${VKR_DEFINITIONS})
target_compile_definitions("VKR" PUBLIC "$<$<CONFIG:Debug>:${VKR_DEBUG_DEFINITIONS}>")
target_compile_definitions("VKR" PUBLIC "$<$<CONFIG:RelWithDebInfo>:VKR_DEBUG=1>")	 # Allow Debug Features in RelWithDbgInfo configuration
