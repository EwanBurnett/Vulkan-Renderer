#include "../include/VKR/Window.h"
#include "../include/VKR/Logger.h"
#include <easy/profiler.h>

VKR::Status VKR::Window::Create(const char* title, const uint32_t width, const uint32_t height)
{
    EASY_FUNCTION(profiler::colors::Green500);

    Log::Debug("[VKR]\tCreating Window \"%s\" (%dx%d)\n", title, width, height); 
    m_Width = width;
    m_Height = height;
    m_Title = title;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_Handle = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (m_Handle == nullptr) {
        Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "[VKR]\tFailed to create GLFW Window! Was glfwInit() called?\n");
        return FAILED;
    }
    Log::Debug("[VKR]\tInternal Window Handle: %08x\n", m_Handle);

    return SUCCESS; 
}

VKR::Status VKR::Window::Destroy()
{
    EASY_FUNCTION(profiler::colors::Green500);
    glfwDestroyWindow(m_Handle);
    m_Handle = nullptr;     //Invalidate the handle
    return SUCCESS; 
}

void VKR::Window::Show()
{
    glfwShowWindow(m_Handle); 
}

void VKR::Window::Hide()
{
    glfwHideWindow(m_Handle); 
}

const uint32_t VKR::Window::GetWidth() const
{
    return m_Width;
}

void VKR::Window::SetWidth(const uint32_t width)
{
    m_Width = width; 
}

const uint32_t VKR::Window::GetHeight() const
{
    return m_Height;
}

void VKR::Window::SetHeight(const uint32_t height)
{
    m_Height = height; 
}


void VKR::Window::SetTitle(const char* title)
{
    glfwSetWindowTitle(m_Handle, title); 
}

GLFWwindow* VKR::Window::GLFWHandle() const
{
    return m_Handle;
}

const bool VKR::Window::PollEvents() const
{
    EASY_FUNCTION(profiler::colors::Green500);
    if (!glfwWindowShouldClose(m_Handle)) {
        glfwPollEvents();
        return true;
    }

    return false;
}