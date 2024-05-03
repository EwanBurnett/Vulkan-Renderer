#include <VKR/VKR.h>
#include <VKR/Window.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

constexpr uint32_t WINDOW_WIDTH = 1280;
constexpr uint32_t WINDOW_HEIGHT = 720;

int main() {
    //Initialize the Application. 
    VKR::Init();

    //EASY_PROFILER_ENABLE;

    //Create a Window. 
    VKR::Window window;
    window.Create("VKR Sample 02 - Mesh Rendering", WINDOW_WIDTH, WINDOW_HEIGHT);
    //Set the window icon.
    {
        GLFWimage icon;
        icon.pixels = stbi_load("Data/VKR_Icon.png", &icon.width, &icon.height, nullptr, 0);
        glfwSetWindowIcon(window.GLFWHandle(), 1, &icon);
    }


    //Run the Render Loop
    while (window.PollEvents()) {
        //TODO: Update Application State
    }
    printf("\n");   //Print a newline for correct log output. 

    window.Hide();

    window.Destroy();

    VKR::Shutdown();
    return 0;
}

