#ifndef PRIMITIVE
#define PRIMITIVE

#include <glm/glm.hpp>

namespace jGL
{

    /** 
     * @brief Higher priority is drawn last.
     * @typedef RenderPriority 
     * */
    typedef uint64_t RenderPriority;

    struct Primitive
    {
        virtual ~Primitive() = default;
    };

    /**
     * @brief Particle with a texture, with a position, orientation, and scale, colour, and texture region.
     * 
     */
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

    /**
     * @brief Position, rotation, and scale.
     * 
     */
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

    /**
     * @brief Rectangular region of a texture.
     * 
     * @param tx Lower left corner of region, along width.
     * @param ty Lower left corner of region, along height.
     * @param lx Length of region along width.
     * @param ly Length of region along height.
     * 
     * @remark Negative lx and ly will be handled as the maximum lengths in e.g.
     * SpriteRenderer.
     */
    struct TextureOffset : public Primitive
    {

        TextureOffset(float x, float y, float w, float h)
        : tx(x), ty(y), lx(w), ly(h)
        {}

        TextureOffset()
        : tx(0.0f), ty(0.0f), lx(-1.0), ly(-1.0)
        {}
        
        float tx; 
        float ty;
        float lx;
        float ly;
    };

}

#endif /* PRIMITIVE */
