#ifndef __VKRENDERER_WINDOW_H
#define __VKRENDERER_WINDOW_H
/**
*   @file Window.h
*   @brief A Wrapper around GLFW, abstracting window management. 
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/23
*/
#include "Types.h"
#include <GLFW/glfw3.h>

namespace VKR {

    class Window {
    public:
        /**
         * @brief Creates a Window. 
         * @param title 
         * @param width 
         * @param height 
         * @return 
        */
        Status Create(const char* title, const uint32_t width, const uint32_t height);

        /**
         * @brief Destroys a Window. 
         * @return 
        */
        Status Destroy();

        void Show(); 
        void Hide(); 

        const uint32_t GetWidth() const;
        void SetWidth(const uint32_t width); 
        const uint32_t GetHeight() const;
        void SetHeight(const uint32_t height); 

        void SetTitle(const char* title);

        /**
         * @brief Returns a window's internal GLFW Handle. 
         * @return 
        */
        GLFWwindow* GLFWHandle() const;

        /**
         * @brief Polls OS Events for this window instance. 
         * @return true after polling events, false on window destruction.
        */
        const bool PollEvents() const;
    private:
        GLFWwindow* m_Handle;

        uint32_t m_Width;
        uint32_t m_Height;

        const char* m_Title;
    };

}
#endif