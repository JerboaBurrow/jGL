#ifndef SPRITE
#define SPRITE

#include <jGL/texture.h>
#include <jGL/primitive.h>

#include <memory>

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
            TextureOffset to
        )
        : transform(tra), texOffset(to)
        {}

        inline virtual void update(Transform tra) { transform = tra; }
        inline virtual void update(TextureOffset to) { texOffset = to; }
        inline virtual void update(Transform tra, TextureOffset to) { update(tra); update(to); }

        const Transform & getTransform() const { return transform; }
        const TextureOffset & getTextureOffset() const { return texOffset; }

    protected:

        Transform transform;
        TextureOffset texOffset;
    
    };
}

#endif /* SPRITE */
