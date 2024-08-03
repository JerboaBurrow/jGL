#include <jGL/primitive.h>

std::ostream & operator <<(std::ostream & out, const jGL::ScreenBoundingBox & sbb)
{
    for (auto p : sbb.vertices)
    {
        out << "(" << unsigned(p.x) << ", " << unsigned(p.y) << ")\n";
    }
    return out;
}

std::ostream & operator <<(std::ostream & out, const jGL::WorldBoundingBox & wbb)
{
    for (auto p : wbb.vertices)
    {
        out << "(" << p.x << ", " << p.y << ")\n";
    }
    return out;
}