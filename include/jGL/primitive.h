#ifndef PRIMITIVE
#define PRIMITIVE

#include <glm/glm.hpp>

namespace jGL
{

    typedef uint64_t RenderPriority;

    struct Primitive
    {
        virtual ~Primitive() = default;
    };

    struct TexturedParticle : public Primitive
    {
        TexturedParticle() = default;
        
        TexturedParticle(glm::vec4 p, glm::vec4 c, glm::vec4 t)
        : state(p), colour(c), texCoord(t)
        {}

        glm::vec4 state;
        glm::vec4 colour;
        glm::vec4 texCoord;
    };

    struct Transform : public Primitive
    {

        Transform(double x, double y, double t, double s)
        : x(x), y(y), theta(t), scale(s)
        {}

        Transform()
        : x(0.0f), y(0.0f), theta(0.0f), scale(0.0f)
        {}

        double x;
        double y;
        double theta;
        double scale;
    };

    struct TextureOffset : public Primitive
    {

        TextureOffset(float x, float y)
        : tx(x), ty(y)
        {}

        TextureOffset()
        : tx(0.0f), ty(0.0f)
        {}
        
        float tx; 
        float ty;
    };

}

#endif /* PRIMITIVE */
