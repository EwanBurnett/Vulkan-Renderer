#include <Renderer.h>
#include <Window.h>
#include <cstdio> 

constexpr uint32_t WINDOW_WIDTH = 600; 
constexpr uint32_t WINDOW_HEIGHT = 400; 

int main() {
    glfwInit(); 

    VKR::Window window; 
    window.Create("Vulkan Renderer", WINDOW_WIDTH, WINDOW_HEIGHT);
    window.Show(); 

    VKR::Renderer renderer; 
    renderer.Init(); 

    uint64_t frameIdx = 0;  //Keep track of the current frame. 
    while (window.PollEvents()) {
        printf("\rFrame %d", frameIdx++);
    }


    renderer.Shutdown(); 

    glfwTerminate(); 

    return 0; 
}