#include <jGL/spriteRenderer.h>

namespace jGL
{
    Sprite & SpriteRenderer::getSprite(SpriteId id)
    {
        return sprites[id];
    }

    void SpriteRenderer::add(Sprite s, SpriteId id)
    {
        Id textureId = s.getTextureId();

        auto is_equal = [textureId](std::shared_ptr<Texture> t) { return t->getId() == textureId; };
        auto match = std::find_if(textureSlots.begin(), textureSlots.end(), is_equal);

        if (match == textureSlots.end())
        {
            if (usedTextureSlots < MAX_TEXTURE_SLOTS)
            {
                textureSlots.push_back(s.getTexture());
                usedTextureSlots += 1;
            }
            else
            {
                throw std::runtime_error("Sprite renderer hit maximum texture slots");
            }
        }

        sprites[id] = s;
    }
}