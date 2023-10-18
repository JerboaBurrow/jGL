#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <iostream>
#include <jGL/OpenGL/gl.h>
#include <jGL/OpenGL/Text/type.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace jGL::GL
{

  class TextRenderer 
  {

  public:

    TextRenderer();

    ~TextRenderer()
    {
      glDeleteBuffers(1,&VBO);
      glDeleteVertexArrays(1,&VAO);
      glDeleteProgram(shader);
    }

    void renderText(
      Type type,
      std::string text,
      float x,
      float y,
      float scale,
      glm::vec3 colour,
      float alpha = 1.0,
      bool centre = false);

      void setProjection(glm::mat4 p)
      {
        
        projection = p;

        glUseProgram(shader);

        glUniformMatrix4fv(
          glGetUniformLocation(shader,"proj"),
          1,
          GL_FALSE,
          &projection[0][0]
        );
      }
  private:

    GLuint shader;
    GLuint VAO;
    GLuint VBO;

    glm::mat4 projection;

    const char * defaultVertexShader = "#version " GLSL_VERSION "\n"
      "in vec4 postex;\n"
      "out vec2 texCoords;\n"
      "uniform mat4 proj;\n"
      "void main()\n"
      "{\n"
      " gl_Position = proj*vec4(postex.xy,0.0,1.0);\n"
      " texCoords = postex.zw;\n"
      "}";

    const char * defaultFragmentShader = "#version " GLSL_VERSION "\n"
      "in vec2 texCoords; out vec4 colour;\n"
      "uniform sampler2D glyph;\n"
      "uniform vec3 textColour;\n"
      "uniform float alpha;\n"
      "void main()\n"
      "{\n"
      " vec4 glpyhSample = vec4(1.0,1.0,1.0,texture(glyph,texCoords).r);\n"
      " colour = vec4(textColour,alpha)*glpyhSample;\n"
      "}";
  };
}
#endif
