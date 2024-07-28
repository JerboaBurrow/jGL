#include <jGL/spriteRenderer.h>

namespace jGL
{
    Sprite & SpriteRenderer::getSprite(SpriteId id)
    {
        return sprites.at(id);
    }

    void SpriteRenderer::add(Sprite s, SpriteId id, RenderPriority priority)
    {
        Id textureId = s.texture->getId();
        auto is_equal = [textureId](std::shared_ptr<Texture> t) { return t->getId() == textureId; };
        auto match = std::find_if(textureSlots.begin(), textureSlots.end(), is_equal);

        if (match == textureSlots.end())
        {
            if (usedTextureSlots < MAX_TEXTURE_SLOTS)
            {
                textureSlots.push_back(s.texture);
                usedTextureSlots += 1;
            }
            else
            {
                throw std::runtime_error("Sprite renderer hit maximum texture slots");
            }
        }

        sprites.insert(std::pair(id, s));
        ids.insert(std::pair(priority, id));
    }
}