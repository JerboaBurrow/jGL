#ifndef EVENT
#define EVENT

#include <GLFW/glfw3.h>

namespace jGL
{
    
    enum class EventType {NONE, PRESS, RELEASE, HOLD, PRESS_OR_HOLD};

    bool operator==(const EventType & lhs, const EventType & rhs);
    
    EventType glfwCodeToEvent(int action);

    struct Event
    {
        Event(double x, double y, EventType t)
        : mouseX(x), mouseY(y), type(t)
        {}

        Event()
        : mouseX(0.0), mouseY(0.0), type(EventType::NONE)
        {}

        double mouseX;
        double mouseY;
        EventType type;
    };
}

#endif /* EVENT */
