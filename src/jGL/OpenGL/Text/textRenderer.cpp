#include <jGL/OpenGL/Text/textRenderer.h>

namespace jGL::GL
{

  TextRenderer::TextRenderer()
  {

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*6*4,NULL,GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,4*sizeof(float),0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glError("TextRenderer constructor: ");

    shader = glCreateProgram();
    compileShader(shader,defaultVertexShader,defaultFragmentShader);

     glError("construct text renderer");

  }

  void TextRenderer::renderText(
    Type type,
    std::string text,
    float x,
    float y,
    float scale,
    glm::vec3 colour,
    float alpha,
    bool centre)
    {
      // have a look at this https://learnopengl.com/In-Practice/Text-Rendering
      // Some modifications have been made, e.g to render \n characters as line breaks

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
      glUseProgram(shader);
      glUniform3f(glGetUniformLocation(shader, "textColour"), colour.x, colour.y, colour.z);
      glError("Setting textColour uniform for type: ");
      glUniform1i(glGetUniformLocation(shader, "glyph"), 0);
      glError("Setting texture id uniform for type: ");
      glUniform1f(glGetUniformLocation(shader, "alpha"), alpha);
      glActiveTexture(GL_TEXTURE0);
      glBindVertexArray(VAO);

      float initialX = x;
      float centreX = 0;
      std::string::const_iterator c;
      if (centre)
      {

        // iterate through all characters
      
        for (c = text.begin(); c != text.end(); c++/*ayy lmao*/)
        {
            Glyph ch = type[*c];
            if (*c == '\n') {
                break;
            }
            float advance = (ch.offset >> 6) * scale;  // bitshift by 6 to get value in pixels (2^6 = 64)
            x += advance;
        }

        centreX = (x - initialX)/2.0;

        initialX -= centreX;
        x = initialX;
      }

      // iterate through all characters
      for (c = text.begin(); c != text.end(); c++/*ayy lmao*/)
      {
          Glyph ch = type[*c];

          float xpos = x + ch.bearing.x * scale;
          float ypos = y - (ch.size.y - ch.bearing.y) * scale;

          float w = ch.size.x * scale;
          float h = ch.size.y * scale;

                  // quick and dirty line break
          if (*c == '\n')
          {
              y -= h*1.1;
              x = initialX;
              continue;
          }
          
          // update VBO for each character
          float vertices[6][4] = {
              { xpos,     ypos + h,   0.0f, 0.0f },
              { xpos,     ypos,       0.0f, 1.0f },
              { xpos + w, ypos,       1.0f, 1.0f },

              { xpos,     ypos + h,   0.0f, 0.0f },
              { xpos + w, ypos,       1.0f, 1.0f },
              { xpos + w, ypos + h,   1.0f, 0.0f }
          };

          // render glyph texture over quad
          glBindTexture(GL_TEXTURE_2D, ch.textureID);
          // update content of VBO memory
          glBindBuffer(GL_ARRAY_BUFFER, VBO);
          glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
          glError("Setting data for Glyph texture " + std::to_string(*c) + " :");
          glBufferStatus("Setting data for Glyph texture "+ std::to_string(*c) + " :");
          // render quad
          glDrawArrays(GL_TRIANGLES, 0, 6);
          // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
          x += (ch.offset >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
      }

      glBindVertexArray(0);
      glBindTexture(GL_TEXTURE_2D, 0);
      
  }

}