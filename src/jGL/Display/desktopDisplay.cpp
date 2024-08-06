#include <jGL/Display/desktopDisplay.h>
#include <logo.h>

namespace jGL
{

    void parseAction
    (
        GLFWwindow * window,
        int code,
        int action
    )
    {
        auto data = reinterpret_cast<DesktopDisplay::WindowData*>(glfwGetWindowUserPointer(window));

        double x, y;
        glfwGetCursorPos(window,&x,&y);

        Event e(x, y, glfwCodeToEvent(action));

        if (data->events.find(code) == data->events.cend())
        {
            (*data).events[code] = {e};
        }
        else
        {
            (*data).events[code].push_back(e);
        }
    }

    void defaultKeyEventCallback
    (
        GLFWwindow * window,
        int key,
        int scancode,
        int action,
        int mods
    )
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        parseAction(window, key, action);
        
    }

    void defaultMouseButtonCallback
    (
        GLFWwindow * window,
        int button,
        int action,
        int mods
    )
    {
        parseAction(window, button, action);
    }

    void defaultScrollCallback
    (
        GLFWwindow * window,
        double x,
        double y
    )
    {
        auto e = reinterpret_cast<DesktopDisplay::WindowData*>(glfwGetWindowUserPointer(window));
        e->scrolled = true;
        e->scrollX = x;
        e->scrollY = y;
    }

    DesktopDisplay::DesktopDisplay
    (
        glm::ivec2 res,
        const char * title,
        GLFWkeyfun keyCallback,
        GLFWmousebuttonfun mouseButtonCallback,
        GLFWscrollfun mouseScrollCallback,
        const Config conf
    )
    : Display(res), title(title), windowConfig(conf)
    {
        if ( !glfwInit() ) { exit(EXIT_FAILURE); }

        if (windowConfig.VULKAN)
        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }

        glfwWindow = NULL;
        open();
        if ( !isOpen() ) { exit(EXIT_FAILURE); }
        setAsFocus();

        glfwSetKeyCallback(glfwWindow, keyCallback);
        glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);
        glfwSetScrollCallback(glfwWindow, mouseScrollCallback);

        std::vector<std::vector<std::byte>> logos =
        {
            {std::vector(LOGO32.begin(), LOGO32.end())},
            {std::vector(LOGO64.begin(), LOGO64.end())},
            {std::vector(LOGO128.begin(), LOGO128.end())}
        };
        
        setIcon(logos);

        data.clear();

        glfwSetWindowUserPointer(glfwWindow, &this->data);
    }

    DesktopDisplay::DesktopDisplay
    (
        glm::ivec2 res,
        const char * title,
        const Config conf
    )
    : DesktopDisplay
    (
        res,
        title,
        defaultKeyEventCallback, 
        defaultMouseButtonCallback, 
        defaultScrollCallback, 
        conf
    )
    {}
}