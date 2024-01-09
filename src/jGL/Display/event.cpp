#include <jGL/Display/event.h>

namespace jGL
{
    bool operator==(const EventType & lhs, const EventType & rhs)
    {
        int li = static_cast<int>(lhs);
        int ri = static_cast<int>(rhs);
        int pi = static_cast<int>(EventType::PRESS);
        int hi = static_cast<int>(EventType::HOLD);
        int phi = static_cast<int>(EventType::PRESS_OR_HOLD);
        if 
        (
            ((li == pi || li == hi) && ri == phi) ||
            ((ri == pi || ri == hi) && li == phi)
        )
        {
            return true;
        }

        return li == ri;
    }
    
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