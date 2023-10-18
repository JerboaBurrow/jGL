#ifndef JGL
#define JGL

#include <memory>

#include <jGL/Display/display.h>

#include <jGL/texture.h>
#include <jGL/particles.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace jGL
{

    class jGLInstance
    {

    public:

        jGLInstance(const Display & display)
        : width(display.getResX()), height(display.getResY())
        {}

        virtual void text
        (
            std::string characters, 
            glm::vec2 position,
            float scale,
            glm::vec4 colour
        ) = 0;

        virtual void clear() = 0;

        virtual void setClear(glm::vec4 colour) = 0;
        virtual void setProjection(glm::mat4 proj) = 0;
        virtual void setTextProjection(glm::mat4 proj) = 0;

        virtual std::shared_ptr<Particles> createParticles(size_t sizeHint) = 0;
        virtual std::shared_ptr<Texture> createTexture(std::string imageFile, Texture::Type type) = 0;

    private:

        uint32_t width, height;

    };
}

#endif /* JGL */
