#include <jGL/OpenGL/glSpriteRenderer.h>

namespace jGL::GL
{
    void glSpriteRenderer::draw(std::shared_ptr<Shader> shader, std::multimap<RenderPriority, SpriteId> ids)
    {

        uint32_t n = ids.size();

        if (offsets.size() < 4*n)
        {
            offsets.resize(4*n+padSprites*4);
            textureOffsets.resize(3*n+padSprites*3);
            freeGL();
            initGL();
        }

        uint64_t i = 0;
        for (auto & sid : ids)
        {
            const Sprite & sprite = sprites[sid.second];
            const Transform & trans = sprite.getTransform();
            const TextureOffset & toff = sprite.getTextureOffset();
            const Id & texId = sprite.getTextureId();
            
            auto is_equal = [texId] (std::shared_ptr<Texture> tex) { return tex->getId() == texId; };

            auto found = std::find_if(textureSlots.begin(), textureSlots.end(), is_equal);
            size_t index = std::distance(textureSlots.begin(), found);
            if (index == textureSlots.size())
            {
                throw std::runtime_error("sprite texture not found in sprite renderer");
            }

            offsets[i*4] = trans.x;
            offsets[i*4+1] = trans.y;
            offsets[i*4+2] = trans.theta;
            offsets[i*4+3] = trans.scale;
            textureOffsets[i*3] = toff.tx;
            textureOffsets[i*3+1] = toff.ty;
            textureOffsets[i*3+2] = float(index);
            
            i += 1;
        }

        for (unsigned i = 0; i < MAX_TEXTURE_SLOTS; i++)
        {
            if (i >= usedTextureSlots)
            {
                break;
            }
            textureSlots[i]->bind(i);
        }

        shader->use();

        shader->setUniform<glm::mat4>("proj", projection);
        shader->setUniform<Sampler2D>("sampler0", Sampler2D(0));
        shader->setUniform<Sampler2D>("sampler1", Sampler2D(1));
        shader->setUniform<Sampler2D>("sampler2", Sampler2D(2));
        shader->setUniform<Sampler2D>("sampler3", Sampler2D(3));

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
            
            glBindBuffer(GL_ARRAY_BUFFER, a_textureOffset);

                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    3*n*sizeof(float),
                    &textureOffsets[0]
                );

            glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        glBindVertexArray(vao);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);
        
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, n);

        glBindVertexArray(0);

        glError("sprite draw");

    }

    void glSpriteRenderer::draw(std::shared_ptr<Shader> shader, std::vector<SpriteId> ids)
    {

        uint32_t n = ids.size();

        if (offsets.size() < 4*n)
        {
            offsets.resize(4*n+padSprites*4);
            textureOffsets.resize(3*n+padSprites*3);
            freeGL();
            initGL();
        }

        uint64_t i = 0;
        for (auto & sid : ids)
        {
            const Sprite & sprite = sprites[sid];
            const Transform & trans = sprite.getTransform();
            const TextureOffset & toff = sprite.getTextureOffset();
            const Id & texId = sprite.getTextureId();
            
            auto is_equal = [texId] (std::shared_ptr<Texture> tex) { return tex->getId() == texId; };

            auto found = std::find_if(textureSlots.begin(), textureSlots.end(), is_equal);
            size_t index = std::distance(textureSlots.begin(), found);
            if (index == textureSlots.size())
            {
                throw std::runtime_error("sprite texture not found in sprite renderer");
            }

            offsets[i*4] = trans.x;
            offsets[i*4+1] = trans.y;
            offsets[i*4+2] = trans.theta;
            offsets[i*4+3] = trans.scale;
            textureOffsets[i*3] = toff.tx;
            textureOffsets[i*3+1] = toff.ty;
            textureOffsets[i*3+2] = float(index);
            
            i += 1;
        }

        for (unsigned i = 0; i < MAX_TEXTURE_SLOTS; i++)
        {
            if (i >= usedTextureSlots)
            {
                break;
            }
            textureSlots[i]->bind(i);
        }

        shader->use();

        shader->setUniform<glm::mat4>("proj", projection);
        shader->setUniform<Sampler2D>("sampler0", Sampler2D(0));
        shader->setUniform<Sampler2D>("sampler1", Sampler2D(1));
        shader->setUniform<Sampler2D>("sampler2", Sampler2D(2));
        shader->setUniform<Sampler2D>("sampler3", Sampler2D(3));

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
            
            glBindBuffer(GL_ARRAY_BUFFER, a_textureOffset);

                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    3*n*sizeof(float),
                    &textureOffsets[0]
                );

            glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        glBindVertexArray(vao);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);
        
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, n);

        glBindVertexArray(0);

        glError("sprite draw");

    }

    void glSpriteRenderer::freeGL()
    {
        glDeleteBuffers(1, &a_position);
        glDeleteBuffers(1, &a_offset);
        glDeleteBuffers(1, &a_textureOffset);
        glDeleteVertexArrays(1, &vao);
    }

    void glSpriteRenderer::initGL()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &a_position);
        glGenBuffers(1, &a_offset);
        glGenBuffers(1, &a_textureOffset);

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

            glBindBuffer(GL_ARRAY_BUFFER, a_textureOffset);

                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    sizeof(float)*textureOffsets.size(),
                    textureOffsets.data(),
                    GL_DYNAMIC_DRAW
                );

                glEnableVertexAttribArray(2);
                glVertexAttribPointer
                (
                    2,
                    3,
                    GL_FLOAT,
                    false,
                    3*sizeof(float),
                    0
                );
                glVertexAttribDivisor(2, 1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);


        glBindVertexArray(0);
    }
}