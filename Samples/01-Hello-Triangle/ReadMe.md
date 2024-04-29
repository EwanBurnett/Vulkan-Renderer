Samples                        {#subpage}
===========

# 01 - Hello Triangle
<!--Demo Image--> 
![image](https://github.com/EwanBurnett/Vulkan-Renderer/assets/25666480/fae6312b-5127-4538-8c15-cd7013888d1b)
- Demonstrates using VKR to initialize Vulkan
- Draws a Triangle to the screen
- Options for viewing Hardware Information, Application Statistics, Vulkan Statistics and showing ImGui Demo.

## Build Instructions
Clone VKR 
```
git clone https://github.com/EwanBurnett/Vulkan-Renderer.git
cd Vulkan-Renderer
```

Generate Project Files with CMake, with the `VKR_BUILD_SAMPLES` option enabled.
```
mkdir build && cd build
cmake .. -DVKR_BUILD_SAMPLES=ON
```

Build using your Platform's build system. 

## Features
### Profiling via EasyProfiler
<!--EasyProfiler Example--> 
![image](https://github.com/EwanBurnett/Vulkan-Renderer/assets/25666480/10a63735-bf2c-42b3-b3c4-6201d7018db5)

- Application must be allowed network accesss
- Profiling Application Initialization requires `EASY_PROFILER_ENABLE` to be called during startup. 
```diff
<main.cpp>
int main() {
    //Initialize the Application. 
    VKR::Init();

-   //EASY_PROFILER_ENABLE
+   EASY_PROFILER_ENABLE
    //...
```


### MultiSample Anti-Aliasing
<!--MSAA Comparison--> 
![Untitled-1](https://github.com/EwanBurnett/Vulkan-Renderer/assets/25666480/ed522705-d71a-4ed6-8859-1b5543056669)
- Compares MSAA count to number of passed fragments
- More noticeable in 3D demos

## Resource Sources
ImGui Font: [Noto Sans JP](https://fonts.google.com/noto/specimen/Noto+Sans+JP)
