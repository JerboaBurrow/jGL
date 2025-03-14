#include <jGL/OpenGL/glSpriteRenderer.h>

namespace jGL::GL
{
    void glSpriteRenderer::draw
    (
        std::shared_ptr<Shader> shader,
        std::vector<std::pair<Info, Sprite>> & sprites
    )
    {

        uint32_t n = idToElement.size();

        if (xytheta.size() < xythetaDim*n)
        {
            xytheta.resize(xythetaDim*n+padSprites*xythetaDim);
            scale.resize(scaleDim*n+padSprites*scaleDim);
            textureRegion.resize(textureRegionDim*n+padSprites*textureRegionDim);
            textureOptions.resize(textureOptionsDim*n+padSprites*textureOptionsDim);
            freeGL();
            initGL();
        }

        std::vector<std::pair<size_t, std::vector<Texture *>>> batches;
        batches.reserve(n);

        uint64_t i = 0;
        std::vector<Texture *> batch;

        for (auto & sprite : sprites)
        {
            size_t textureIndex;
            Id textureId = sprite.second.texture->getId();
            auto is_equal = [textureId](Texture * t) { return t->getId() == textureId; };
            auto match = std::find_if(batch.begin(), batch.end(), is_equal);
            if (match == batch.end())
            {
                if (batch.size() == MAX_BATCH_BOUND_TEXTURES)
                {
                    batches.push_back(std::pair(i, batch));
                    batch.clear();
                }
                batch.push_back(sprite.second.texture);
                textureIndex = batch.size()-1;
            }
            else
            {
                textureIndex = std::distance(batch.begin(), match);
            }

            const NormalisedTextureRegion toff = sprite.second.getNormalisedTextureRegion();

            xytheta[i*xythetaDim] = sprite.second.transform->x;
            xytheta[i*xythetaDim+1] = sprite.second.transform->y;
            xytheta[i*xythetaDim+2] = sprite.second.transform->theta;

            scale[i*scaleDim] = sprite.second.transform->scaleX;
            scale[i*scaleDim+1] = sprite.second.transform->scaleY;

            textureRegion[i*textureRegionDim] = toff.tx;
            textureRegion[i*textureRegionDim+1] = toff.ty;
            textureRegion[i*textureRegionDim+2] = toff.lx;
            textureRegion[i*textureRegionDim+3] = toff.ly;

            textureOptions[i*textureOptionsDim] = float(textureIndex);
            textureOptions[i*textureOptionsDim+1] = sprite.second.colour->a;

            i++;
        }
        batches.push_back(std::pair(i, batch));

        uint64_t b = 0;
        uint64_t current = 0;
        uint64_t next = batches[b].first-1;
        std::vector<Texture *>::const_iterator biter;

        while (b < batches.size())
        {
            biter = batches[b].second.cbegin();
            for (unsigned slot = 0; slot < MAX_BATCH_BOUND_TEXTURES; slot++)
            {
                if (biter == batches[b].second.cend()) { break; }
                (*biter)->bind(slot);
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

            glBindBuffer(GL_ARRAY_BUFFER, a_xytheta);
                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    xythetaDim*batchSize*sizeof(float),
                    &xytheta[current*xythetaDim]
                );
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_scale);

                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    scaleDim*batchSize*sizeof(float),
                    &scale[current*scaleDim]
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
        glDeleteBuffers(1, &a_xytheta);
        glDeleteBuffers(1, &a_scale);
        glDeleteBuffers(1, &a_textureRegion);
        glDeleteBuffers(1, &a_textureOption);
        glDeleteVertexArrays(1, &vao);
    }

    void glSpriteRenderer::initGL()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &a_position);
        glGenBuffers(1, &a_xytheta);
        glGenBuffers(1, &a_scale);
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

            glBindBuffer(GL_ARRAY_BUFFER, a_xytheta);

                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    sizeof(float)*xytheta.size(),
                    xytheta.data(),
                    GL_DYNAMIC_DRAW
                );

                glEnableVertexAttribArray(xythetaAttribute);
                glVertexAttribPointer
                (
                    xythetaAttribute,
                    xythetaDim,
                    GL_FLOAT,
                    false,
                    xythetaDim*sizeof(float),
                    0
                );
                glVertexAttribDivisor(xythetaAttribute, 1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_scale);

                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    sizeof(float)*scale.size(),
                    scale.data(),
                    GL_DYNAMIC_DRAW
                );

                glEnableVertexAttribArray(scaleAttribute);
                glVertexAttribPointer
                (
                    scaleAttribute,
                    scaleDim,
                    GL_FLOAT,
                    false,
                    scaleDim*sizeof(float),
                    0
                );
                glVertexAttribDivisor(scaleAttribute, 1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_textureRegion);

                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    sizeof(float)*textureRegion.size(),
                    textureRegion.data(),
                    GL_DYNAMIC_DRAW
                );

                glEnableVertexAttribArray(textureRegionAttribute);
                glVertexAttribPointer
                (
                    textureRegionAttribute,
                    textureRegionDim,
                    GL_FLOAT,
                    false,
                    textureRegionDim*sizeof(float),
                    0
                );
                glVertexAttribDivisor(textureRegionAttribute, 1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_textureOption);

                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    sizeof(float)*textureOptions.size(),
                    textureOptions.data(),
                    GL_DYNAMIC_DRAW
                );

                glEnableVertexAttribArray(textureOptionsAttribute);
                glVertexAttribPointer
                (
                    textureOptionsAttribute,
                    textureOptionsDim,
                    GL_FLOAT,
                    false,
                    textureOptionsDim*sizeof(float),
                    0
                );
                glVertexAttribDivisor(textureOptionsAttribute, 1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }
}