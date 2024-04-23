#include "../include/Window.h"

VKR::Status VKR::Window::Create(const char* title, const uint32_t width, const uint32_t height)
{
    m_Width = width;
    m_Height = height;
    m_Title = title;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_Handle = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (m_Handle == nullptr) {
        return FAILED;
    }

    return SUCCESS; 
}

VKR::Status VKR::Window::Destroy()
{
    glfwDestroyWindow(m_Handle);
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
    if (!glfwWindowShouldClose(m_Handle)) {
        glfwPollEvents();
        return true;
    }

    return false;
}