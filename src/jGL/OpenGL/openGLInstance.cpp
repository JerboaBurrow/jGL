#include <jGL/OpenGL/openGLInstance.h>

namespace jGL::GL
{

    void OpenGLInstance::text
    (
        std::string characters, 
        glm::vec2 position,
        float scale,
        glm::vec4 colour,
        glm::bvec2 centre
    )
    {
        textRenderer.renderText(
            defaultFont,
            characters,
            position,
            scale,
            colour,
            resolution,
            centre
        );
    }

}