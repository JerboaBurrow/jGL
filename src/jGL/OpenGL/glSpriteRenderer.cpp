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

        std::vector<size_t> textureIndices(ids.size(), 0);
        std::vector<std::pair<size_t, std::vector<size_t>>> batches;
        batches.reserve(n);

        uint64_t i = 0;
        std::vector<size_t> batch;

        for (auto & sid : ids)
        {
            size_t textureIndex = spriteToTexture[sid.second];
            bool newTexture = std::find(batch.begin(), batch.end(), textureIndex) == batch.end();
            if (newTexture)
            {
                if (batch.size() == MAX_TEXTURE_SLOTS)
                {
                    batches.push_back(std::pair(i, batch));
                    batch.clear();
                }
                batch.push_back(textureIndex);
            }

            const Sprite & sprite = sprites.at(sid.second);
            const Transform & trans = sprite.transform;
            const TextureOffset toff = sprite.getTextureOffset(true);
            const float alpha = sprite.getAlpha();

            offsets[i*offsetDim] = trans.x;
            offsets[i*offsetDim+1] = trans.y;
            offsets[i*offsetDim+2] = trans.theta;
            offsets[i*offsetDim+3] = trans.scale;
            textureRegion[i*textureRegionDim] = toff.tx;
            textureRegion[i*textureRegionDim+1] = toff.ty;
            textureRegion[i*textureRegionDim+2] = toff.lx;
            textureRegion[i*textureRegionDim+3] = toff.ly;
            textureOptions[i*textureOptionsDim] = float(std::distance(batch.begin(), std::find(batch.begin(), batch.end(), textureIndex)));
            textureOptions[i*textureOptionsDim+1] = alpha;

            i++;
        }
        batches.push_back(std::pair(i, batch));

        uint64_t b = 0;
        uint64_t current = 0;
        uint64_t next = batches[b].first-1;
        std::vector<size_t>::const_iterator biter;

        while (b < batches.size())
        {
            biter = batches[b].second.cbegin();
            for (unsigned slot = 0; slot < MAX_TEXTURE_SLOTS; slot++)
            {
                if (biter == batches[b].second.cend()) { break; }
                textures[*biter]->bind(slot);
                biter++;
            }

            shader->use();

            shader->setUniform<glm::mat4>("proj", projection);
            shader->setUniform<Sampler2D>("sampler0", Sampler2D(0));
            shader->setUniform<Sampler2D>("sampler1", Sampler2D(1));
            shader->setUniform<Sampler2D>("sampler2", Sampler2D(2));
            shader->setUniform<Sampler2D>("sampler3", Sampler2D(3));

            size_t batchSize = next-current+1;

            glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, a_offset);
                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    offsetDim*batchSize*sizeof(float),
                    &offsets[current*offsetDim]
                );
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            glBindBuffer(GL_ARRAY_BUFFER, a_textureRegion);

                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    textureRegionDim*batchSize*sizeof(float),
                    &textureRegion[current*textureRegionDim]
                );

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_textureOption);

                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    textureOptionsDim*batchSize*sizeof(float),
                    &textureOptions[current*textureOptionsDim]
                );

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindVertexArray(0);

            glBindVertexArray(vao);

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDisable(GL_DEPTH_TEST);

                glDrawArraysInstanced(GL_TRIANGLES, 0, 6, batchSize);

            glBindVertexArray(0);

            glError("sprite draw");

            if (b+1 == batches.size()){ break; }

            current = batches[b].first;
            b++;
            next = batches[b].first-1;
        }

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