Samples                        {#subpage}
===========

# 01 - Hello Triangle
<!--Demo Image--> 

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

## Options
Right click within the Overlay to open an options menu. 

<!--Options Menu-->
### Hardware Information
<!--Hardware Information Menu--> 

### Application Statistics
<!--Application Statistics Menu--> 

### Vulkan Statistics
<!--Vulkan Statistics Menu--> 

### ImGui Demo
<!--ImGui Demo--> 


## Resource Sources
ImGui Font: [Noto Sans JP](https://fonts.google.com/noto/specimen/Noto+Sans+JP)
