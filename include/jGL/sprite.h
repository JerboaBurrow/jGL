#ifndef SPRITE_H
#define SPRITE_H

#include <jGL/texture.h>
#include <jGL/primitive.h>

#include <memory>
#include <algorithm>

/*

    A sprite holds a constant transform reference (position, rotation, scale),
     texture offsets (tx, ty, unit), a shared texture pointer, and an alpha
     value.

    The position, rotation and scale define where a sprite is drawn as observed
     by the constant reference. The intention is to store e.g. in Hop's ECS.

    The texture offsets can be used for atlasing, and selecting a texture
     unit in the shader. Shaders are applied from the SpriteRenderer.

    void setAlpha(float) - controls setting of alpha, with clamp(a, 0, 1)

*/

namespace jGL
{
    class Sprite
    {
    public:

        Sprite
        ( 
            const Transform & tra,
            TextureOffset to,
            std::shared_ptr<Texture> tex,
            float alpha = 1.0f
        )
        : transform(tra), texOffset(to), texture(tex), alpha(std::clamp(alpha, 0.0f, 1.0f))
        {}

        inline virtual void setAlpha(float a) { alpha = std::clamp(a, 0.0f, 1.0f); }
        const float getAlpha() const { return alpha; }

        const Transform & transform;
        TextureOffset texOffset;
        std::shared_ptr<Texture> texture;

    protected:

        float alpha;
    
    };
}

#endif /* SPRITE_H */
