#include <jGL/OpenGL/openGLInstance.h>

namespace jGL::GL
{

    void OpenGLInstance::text
    (
        std::string characters, 
        glm::vec2 position,
        float scale,
        glm::vec4 colour
    )
    {
        textRenderer.renderText(
            defaultFont,
            characters,
            position.x,
            position.y,
            scale,
            glm::vec3(colour.r, colour.g, colour.b),
            colour.a
        );

    }

}