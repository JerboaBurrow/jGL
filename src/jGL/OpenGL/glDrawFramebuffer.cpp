#include <jGL/OpenGL/glDrawFramebuffer.h>

namespace jGL::GL
{
    bool glDrawFramebuffer::initialised = false;
    uint8_t glDrawFramebuffer::msaaSamples = 0;
    GLuint glDrawFramebuffer::buffer;
    GLuint glDrawFramebuffer::texture;
    uint16_t glDrawFramebuffer::resX = 0;
    uint16_t glDrawFramebuffer::resY = 0;
    glm::vec4 glDrawFramebuffer::clearColour = glm::vec4(1.0);
}