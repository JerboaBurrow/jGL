#include <jGL/Display/event.h>

namespace jGL
{
    
    EventType glfwCodeToEvent(int action)
    {
        switch (action)
        {
            case GLFW_PRESS:
                return EventType::PRESS;

            case GLFW_RELEASE:
                return EventType::RELEASE;

            case GLFW_REPEAT:
                return EventType::HOLD;

            default:
                return EventType::NONE;
        }
    }
}