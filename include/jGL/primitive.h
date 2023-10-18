#ifndef PRIMITIVE
#define PRIMITIVE

#include <glm/glm.hpp>

namespace jGL
{
    struct Primitive
    {

    };

    struct TexturedParticle : public Primitive
    {
        glm::vec4 state;
        glm::vec4 colour;
        glm::vec4 texCoord;
    };
}

#endif /* PRIMITIVE */
