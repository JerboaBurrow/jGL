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
        auto match = std::find_if(textures.begin(), textures.end(), is_equal);

        if (match == textures.end())
        {
            textures.push_back(s.texture);
            spriteToTexture[id] = textures.size()-1;
        }
        else
        {
            spriteToTexture[id] = std::distance(textures.begin(), match);
        }

        sprites.insert(std::pair(id, s));
        ids.insert(std::pair(priority, id));
    }
}