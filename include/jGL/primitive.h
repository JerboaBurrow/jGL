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

    struct Transform : public Primitive
    {
        float x;
        float y;
        float theta;
        float scale;
    };

    struct TextureOffset : public Primitive
    {
        float tx; 
        float ty;
        uint16_t unit;
    };

}

#endif /* PRIMITIVE */
