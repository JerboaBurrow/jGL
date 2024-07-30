#include <jGL/OpenGL/glSpriteRenderer.h>

namespace jGL::GL
{
    void glSpriteRenderer::draw(std::shared_ptr<Shader> shader, std::multimap<RenderPriority, SpriteId> ids)
    {

        uint32_t n = ids.size();

        if (offsets.size() < offsetDim*n)
        {
            offsets.resize(offsetDim*n+padSprites*offsetDim);
            textureRegion.resize(textureRegionDim*n+padSprites*textureRegionDim);
            textureOptions.resize(textureOptionsDim*n+padSprites*textureOptionsDim);
            freeGL();
            initGL();
        }

        uint64_t i = 0;
        for (auto & sid : ids)
        {
            const Sprite & sprite = sprites.at(sid.second);
            const Transform & trans = sprite.transform;
            const TextureOffset toff = sprite.getTextureOffset(true);
            const Id & texId = sprite.texture->getId();
            const float alpha = sprite.getAlpha();
            
            auto is_equal = [texId] (std::shared_ptr<Texture> tex) { return tex->getId() == texId; };

            auto found = std::find_if(textureSlots.begin(), textureSlots.end(), is_equal);
            size_t index = std::distance(textureSlots.begin(), found);
            if (index == textureSlots.size())
            {
                throw std::runtime_error("sprite texture not found in sprite renderer");
            }

            offsets[i*offsetDim] = trans.x;
            offsets[i*offsetDim+1] = trans.y;
            offsets[i*offsetDim+2] = trans.theta;
            offsets[i*offsetDim+3] = trans.scale;
            textureRegion[i*textureRegionDim] = toff.tx;
            textureRegion[i*textureRegionDim+1] = toff.ty;
            textureRegion[i*textureRegionDim+2] = toff.lx;
            textureRegion[i*textureRegionDim+3] = toff.ly;
            textureOptions[i*textureOptionsDim] = float(index);
            textureOptions[i*textureOptionsDim+1] = alpha;

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
                    offsetDim*n*sizeof(float),
                    &offsets[0]
                );
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            glBindBuffer(GL_ARRAY_BUFFER, a_textureRegion);

                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    textureRegionDim*n*sizeof(float),
                    &textureRegion[0]
                );

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_textureOption);

                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    textureOptionsDim*n*sizeof(float),
                    &textureOptions[0]
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
        glDeleteBuffers(1, &a_textureRegion);
        glDeleteBuffers(1, &a_textureOption);
        glDeleteVertexArrays(1, &vao);
    }

    void glSpriteRenderer::initGL()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &a_position);
        glGenBuffers(1, &a_offset);
        glGenBuffers(1, &a_textureRegion);
        glGenBuffers(1, &a_textureOption);

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
                    offsetDim,
                    GL_FLOAT,
                    false,
                    offsetDim*sizeof(float),
                    0
                );
                glVertexAttribDivisor(1, 1);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_textureRegion);

                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    sizeof(float)*textureRegion.size(),
                    textureRegion.data(),
                    GL_DYNAMIC_DRAW
                );

                glEnableVertexAttribArray(2);
                glVertexAttribPointer
                (
                    2,
                    textureRegionDim,
                    GL_FLOAT,
                    false,
                    textureRegionDim*sizeof(float),
                    0
                );
                glVertexAttribDivisor(2, 1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_textureOption);

                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    sizeof(float)*textureOptions.size(),
                    textureOptions.data(),
                    GL_DYNAMIC_DRAW
                );

                glEnableVertexAttribArray(3);
                glVertexAttribPointer
                (
                    3,
                    textureOptionsDim,
                    GL_FLOAT,
                    false,
                    textureOptionsDim*sizeof(float),
                    0
                );
                glVertexAttribDivisor(3, 1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }
}