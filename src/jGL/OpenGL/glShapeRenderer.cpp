#include <jGL/OpenGL/glShapeRenderer.h>

namespace jGL::GL 
{
    void glShapeRenderer::draw(std::shared_ptr<Shader> shader, std::multimap<RenderPriority, ShapeId> ids)
    {

        uint32_t n = ids.size();

        if (offsets.size() < 4*n)
        {
            offsets.resize(4*n+padShapes*4);
            colours.resize(4*n+padShapes*4);
            freeGL();
            initGL();
        }

        uint64_t i = 0;
        for (auto & sid : ids)
        {
            const auto shape = shapes[sid.second];
            const Transform & trans = shape->getTransform();
            const glm::vec4 & col = shape->getColour();

            offsets[i*4] = trans.x;
            offsets[i*4+1] = trans.y;
            offsets[i*4+2] = trans.theta;
            offsets[i*4+3] = trans.scale;
            colours[i*4] = col.r;
            colours[i*4+1] = col.g;
            colours[i*4+2] = col.b;
            colours[i*4+3] = col.a;

            i += 1;
        }

        shader->use();
        shader->setUniform<glm::mat4>("proj", projection);

        glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, a_offset);
                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    4*n*sizeof(float),
                    &offsets[0]
                );
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            glBindBuffer(GL_ARRAY_BUFFER, a_colour);

                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    4*n*sizeof(float),
                    &colours[0]
                );

            glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        glBindVertexArray(vao);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);
        
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, n);

        glBindVertexArray(0);

    }

    void glShapeRenderer::draw(std::shared_ptr<Shader> shader, std::vector<ShapeId> ids)
    {

        uint32_t n = ids.size();

        if (offsets.size() < 4*n)
        {
            offsets.resize(4*n+padShapes*4);
            colours.resize(4*n+padShapes*4);
            freeGL();
            initGL();
        }

        uint64_t i = 0;
        for (auto & sid : ids)
        {
            const auto shape = shapes[sid];
            const Transform & trans = shape->getTransform();
            const glm::vec4 & col = shape->getColour();

            offsets[i*4] = trans.x;
            offsets[i*4+1] = trans.y;
            offsets[i*4+2] = trans.theta;
            offsets[i*4+3] = trans.scale;
            colours[i*4] = col.r;
            colours[i*4+1] = col.g;
            colours[i*4+2] = col.b;
            colours[i*4+3] = col.a;

            i += 1;
        }

        shader->use();
        shader->setUniform<glm::mat4>("proj", projection);

        glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, a_offset);
                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    4*n*sizeof(float),
                    &offsets[0]
                );
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            glBindBuffer(GL_ARRAY_BUFFER, a_colour);

                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    4*n*sizeof(float),
                    &colours[0]
                );

            glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        glBindVertexArray(vao);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);
        
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, n);

        glBindVertexArray(0);

    }

    void glShapeRenderer::freeGL()
    {
        glDeleteBuffers(1, &a_position);
        glDeleteBuffers(1, &a_offset);
        glDeleteBuffers(1, &a_colour);
        glDeleteVertexArrays(1, &vao);
    }

    void glShapeRenderer::initGL()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &a_position);
        glGenBuffers(1, &a_offset);
        glGenBuffers(1, &a_colour);

        glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, a_position);
                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    sizeof(float)*6*4,
                    &quad[0],
                    GL_STATIC_DRAW
                );
                glEnableVertexAttribArray(0);
                glVertexAttribPointer
                (
                    0,
                    4,
                    GL_FLOAT,
                    false,
                    4*sizeof(float),
                    0
                );
                glVertexAttribDivisor(0,0);
    
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_offset);

                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    sizeof(float)*offsets.size(),
                    offsets.data(),
                    GL_DYNAMIC_DRAW
                );

                glEnableVertexAttribArray(1);
                glVertexAttribPointer
                (
                    1,
                    4,
                    GL_FLOAT,
                    false,
                    4*sizeof(float),
                    0
                );
                glVertexAttribDivisor(1, 1);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_colour);

                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    sizeof(float)*colours.size(),
                    colours.data(),
                    GL_DYNAMIC_DRAW
                );

                glEnableVertexAttribArray(2);
                glVertexAttribPointer
                (
                    2,
                    4,
                    GL_FLOAT,
                    false,
                    4*sizeof(float),
                    0
                );
                glVertexAttribDivisor(2, 1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);


        glBindVertexArray(0);
    }
}