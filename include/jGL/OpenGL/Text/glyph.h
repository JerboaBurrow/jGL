#ifndef VKGLYPH
#define VKGLYPH

#include <jGL/OpenGL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace jGL::GL
{
  struct Glyph 
  {

    Glyph(){}

    Glyph(GLuint & u, glm::ivec2 s, glm::ivec2 b, uint64_t o)
    : textureID(u), size(s), bearing(b), offset(o) {}

    GLuint textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    uint64_t offset;
    
  };
}
#endif /* VKGLYPH */
