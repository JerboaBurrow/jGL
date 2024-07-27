#ifndef SPRITE_H
#define SPRITE_H

#include <jGL/texture.h>
#include <jGL/primitive.h>

#include <memory>
#include <algorithm>

/*

    A sprite holds a transform (position, rotation, scale)
     and texture offsets (tx, ty, unit)

    The position rotation and scale define where a sprite is drawn

    The texture offsets can be used for atlasing, and selecting a texture
     unit in the shader

*/

namespace jGL
{
    class Sprite
    {
    public:

        Sprite()
        {}

        Sprite
        ( 
            Transform tra,
            TextureOffset to,
            std::shared_ptr<Texture> tex,
            float alpha = 1.0f
        )
        : transform(tra), texOffset(to), texture(tex), alpha(std::clamp(alpha, 0.0f, 1.0f))
        {}

        inline virtual void setAlpha(float a) { alpha = std::clamp(a, 0.0f, 1.0f); }
        const float getAlpha() const { return alpha; }

        Transform transform;
        TextureOffset texOffset;
        std::shared_ptr<Texture> texture;

    protected:

        float alpha;
    
    };
}

#endif /* SPRITE_H */
