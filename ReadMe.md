# Vulkan Renderer 
A 3D Renderer written using C++ to explore the Vulkan API. 

## Dependencies
- A C++ 17 Compiler
- [CMake 3.14](https://cmake.org) or higher
- The latest version of the [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/) 

### External Libraries
in addition to the above, as the following libraries are used, their dependencies for your platform must also be installed: 
    - [GLFW v3.4](https://github.com/glfw/glfw.git)
    - [ImGui v1.90.5](https://github.com/ocornut/imgui.git)
    - [Assimp v5.4.0](https://github.com/assimp/assimp.git)
    - [STB](https://github.com/nothings/stb.git)
    - [Easy Profiler v2.1.0](https://github.com/yse/easy_profiler.git)
    - [EnkiTS v1.11](https://github.com/dougbinks/enkiTS.git)
    - [Vulkan Memory Allocator v3.0.1](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git)

## Build Instructions
Clone the repository as follows
```
git clone https://github.com/EwanBurnett/Vulkan-Renderer.git
cd Vulkan-Renderer
```

Generate project files via CMake
```
mkdir build && cd build
cmake .. 
# Build using your platform's build system 
```
