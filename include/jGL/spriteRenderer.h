#ifndef SPRITERENDERER
#define SPRITERENDERER

#include <jGL/sprite.h>
#include <jGL/shader.h>

#include <unordered_map>
#include <string>
#include <iterator>
#include <vector>

namespace jGL
{
    typedef std::string SpriteId;

    class SpriteRenderer
    {

    public:
    
        SpriteRenderer(size_t sizeHint = 64)
        {
            sprites.reserve(sizeHint); 
        }

        virtual Sprite & getSprite(SpriteId id) { return sprites[id]; }

        virtual void draw(std::shared_ptr<Shader> shader, std::vector<SpriteId> ids) = 0;

        virtual void add(Sprite s, SpriteId id) { sprites[id] = s;}

    protected:

        std::unordered_map<SpriteId, Sprite> sprites;

    };
}

#endif /* SPRITERENDERER */
