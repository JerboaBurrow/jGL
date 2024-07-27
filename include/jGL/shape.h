#ifndef SHAPE_H
#define SHAPE_H

#include <jGL/primitive.h>

namespace jGL
{
    class Shape
    {
    
    public:

        Shape()
        {}

        Shape(Transform tra, glm::vec4 c)
        : transform(tra), colour(c)
        {}

        Transform transform;
        glm::vec4 colour;

    };
}

#endif /* SHAPE_H */
