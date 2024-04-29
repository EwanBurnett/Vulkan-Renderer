#include "01-Hello-Triangle.h"

#include <VKR/VKR.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

constexpr uint32_t WINDOW_WIDTH = 600;
constexpr uint32_t WINDOW_HEIGHT = 400;

int main() {
    //Initialize the Application. 
    VKR::Init();

    //EASY_PROFILER_ENABLE;

    //Create a Window. 
    VKR::Window window;
    window.Create("VKR Sample 01 - Hello Triangle", WINDOW_WIDTH, WINDOW_HEIGHT);
    //Set the window icon.
    {
        GLFWimage icon;
        icon.pixels = stbi_load("Data/VKR_Icon.png", &icon.width, &icon.height, nullptr, 0);
        glfwSetWindowIcon(window.GLFWHandle(), 1, &icon);
    }

    Samples::HelloTriangleApp demoApp;
    demoApp.Init(window);

    //Run the Render Loop
    while (window.PollEvents()) {
        demoApp.BeginFrame();

        demoApp.Update();
        demoApp.FixedUpdate();

        demoApp.EndFrame();
    }
    printf("\n");   //Print a newline for correct log output. 

    window.Hide();

    demoApp.Shutdown();
    window.Destroy();

    VKR::Shutdown();
    return 0;
}

