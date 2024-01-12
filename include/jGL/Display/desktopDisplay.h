#ifndef DesktopDisplay_H
#define DesktopDisplay_H

#include <cstdlib>
#include <vector>
#include <map>

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <jGL/Display/event.h>
#include <jGL/Display/display.h>

namespace jGL
{

    void defaultKeyEventCallback
    (
        GLFWwindow * window,
        int key,
        int scancode,
        int action,
        int mods
    );

    class DesktopDisplay : public Display
    {
    public:

        DesktopDisplay
        (
            glm::ivec2 res,
            const char * title,
            GLFWkeyfun keyCallback,
            GLFWmousebuttonfun mouseButtonCallback,
            GLFWscrollfun mouseScrollCallback,
            bool vulkan = false
        );

        DesktopDisplay
        (
            glm::ivec2 res,
            const char * title,
            bool vulkan = false
        );

        ~DesktopDisplay(){ glfwTerminate(); free(logo); }

        GLFWwindow * getWindow() const { return glfwWindow; }

        bool isOpen(){ if (glfwWindow != NULL) { return !glfwWindow ? false : true; } return false; }
        bool closing(){ return glfwWindowShouldClose(glfwWindow); }
        
        void open(){ 
            if (glfwWindow == NULL)
            { 
                // required for MacOS
                //   https://www.glfw.org/faq.html#41__how_do_i_create_an_opengl_30_context
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
                glfwWindow = glfwCreateWindow(getResX(), getResY(),title,NULL,NULL); glfwSwapInterval(1); 
            } 
        }

        void close(){ if (glfwWindow != NULL) { glfwDestroyWindow(glfwWindow); glfwWindow = NULL; } }

        void setAsFocus(){ if (glfwWindow != NULL) { glfwMakeContextCurrent(glfwWindow); } }

        void keyCallback
        (
            GLFWwindow * window,
            int key,
            int scancode,
            int action,
            int mods
        );

        void mousePosition(double & x, double & y){ if (glfwWindow != NULL){ glfwGetCursorPos(glfwWindow,&x,&y); } }

        void setMousePosition(double x, double y)
        {
            if (glfwWindow != NULL)
            {
                glfwSetCursorPos(glfwWindow, x, y);
            }
        }

        int getKeyLastState(int key) { return glfwGetKey(glfwWindow, key); }

        void loop() 
        { 
            data.clear();
            swap(); 
            handleEvents(); 
            if (glfwWindowShouldClose(glfwWindow)){ close(); }
        }

        std::vector<Event> getEvents(int code) 
        {
            if (data.events.find(code) == data.events.cend())
            {
                return {Event()};
            }
            else
            {
                return data.events[code];
            }
        }

        std::vector<EventType> getEventTypes(int code) 
        {
            std::vector<EventType> e;
            if (data.events.find(code) == data.events.cend())
            {
                return {EventType::NONE};
            }
            else
            {
                for (auto evt : data.events[code])
                {
                    e.push_back(evt.type);
                }
                return e;
            }
        }

        Event getEvent(int code)
        {
            if (data.events.find(code) == data.events.cend())
            {
                return Event();
            }
            else
            {
                return data.events[code][0];
            }
        }

        struct WindowData 
        {
            std::map<int, std::vector<Event>> events;
            double scrollX = 0.0;
            double scrollY = 0.0;
            bool scrolled = false;

            void clear()
            {
                scrollX = 0.0;
                scrollY = 0.0;
                scrolled = false;
                events.clear();
            }
        };

    private:

        const char * title;

        GLFWimage * logo;

        GLFWwindow * glfwWindow;

        WindowData data;

        void swap(){ if (glfwWindow != NULL) { glfwSwapBuffers(glfwWindow); } }

        void handleEvents(){ if (glfwWindow != NULL){ glfwPollEvents(); } }

    };
}

#endif /* DesktopDisplay_H */