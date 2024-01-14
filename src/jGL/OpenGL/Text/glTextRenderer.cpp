#include <jGL/OpenGL/Text/glTextRenderer.h>
#include <iostream>

namespace jGL::GL
{

  TextRenderer::TextRenderer(glm::vec2 res)
  : shader(vert, frag), res(res)
  {

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);

    glBindVertexArray(VAO);
      
      glBindBuffer(GL_ARRAY_BUFFER, VBO);

      charactersUploaded = 512;
      vertices = std::vector<float>(charactersUploaded*6*4, 0.0);

      glBufferData(GL_ARRAY_BUFFER,sizeof(float)*vertices.size(),vertices.data(),GL_DYNAMIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,4*sizeof(float),0);

      glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    shader.compile();

  }

  void TextRenderer::setBufferSize(uint16_t s)
  {
    glDeleteBuffers(1, &VBO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
      
      glBindBuffer(GL_ARRAY_BUFFER, VBO);

      charactersUploaded = s;
      vertices = std::vector<float>(charactersUploaded*6*4, 0.0);

      glBufferData(GL_ARRAY_BUFFER,sizeof(float)*vertices.size(),vertices.data(),GL_DYNAMIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,4*sizeof(float),0);

      glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
  }

  void TextRenderer::renderText(
    glFont font,
    std::string text,
    glm::vec2 position,
    float scale,
    glm::vec4 colour,
    glm::vec2 res,
    bool centre)
    {

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
      
      shader.use();
      shader.setUniform<glm::vec4>("textColour", colour);
      shader.setUniform<int>("glyph", 0);
      font.bind(0);
      glActiveTexture(GL_TEXTURE0);

      if (charactersUploaded < text.size())
      {
        setBufferSize(text.size()+32);
      }

      glBindVertexArray(VAO);

        float p = position.x;
        float y = res.y-position.y;
        unsigned i = 0;

        for (unsigned char ch : text)
        {
            if (ch == 0x20)
            {
                p += font.spacing(scale);
            }
            else if (ch == 0x0A)
            {
                y += font.spacing(scale);
                p = position.x;
            }
            else
            {
                for(auto vertex : font.getGlyphVertices(p, y, scale, ch))
                {
                    vertices[i] = vertex.x;
                    vertices[i+1] = -vertex.y+res.y;
                    vertices[i+2] = vertex.z;
                    vertices[i+3] = vertex.w;
                    i+=4;
                }
                p += scale*(font.getGlyphSize(ch).x+1);
            } 
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

          glBufferSubData
          (
            GL_ARRAY_BUFFER,
            0,
            vertices.size()*sizeof(float),
            vertices.data()
          );

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, i);

      glBindVertexArray(0);
      
  }

}