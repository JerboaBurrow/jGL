#ifndef SPRITE_H
#define SPRITE_H

#include <jGL/texture.h>
#include <jGL/primitive.h>

#include <memory>
#include <algorithm>

/**
 * @brief A drawable graphic.
 * 
 * Observes a Transform (position, orientation, scale), and a Texture.
 * 
 * Rendered using the TextureOffset (pixel units) region of the Texture, at the alpha value.
 * 
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
        : transform(tra), texture(tex), alpha(std::clamp(alpha, 0.0f, 1.0f)), texOffset(toNormalised(to))
        {}

        /**
         * @brief Sets the sprite alpha modifier.
         * 
         * @param a alpha value, clamping is applied: std::clamp(a, 0.0f, 1.0f).
         */
        inline virtual void setAlpha(float a) { alpha = std::clamp(a, 0.0f, 1.0f); }

        /**
         * @brief Get the alpha modifer.
         * 
         * @return const float, which is in [0.0, 1.0].
         */
        const float getAlpha() const { return alpha; }

        /**
         * @brief Get the Texture Offset in pixel units.
         * 
         * @param normalised return the texture region in normalised units.
         * 
         * @return TextureOffset (pixel by default).
         */
        TextureOffset getTextureOffset(bool normalised = false) const
        { 
            if (!normalised) {return fromNormalised(texOffset); }
            return texOffset;
        }

        /**
         * @brief Set the Texture Offset from a pixel region.
         * 
         * @remark Values outside of the textures pixel range will be clamped.
         * @remark Lengths lx or ly < 0 will be set to maximum.
         * 
         */
        void setTextureOffset(TextureOffset to) { texOffset = toNormalised(texOffset); }

        const Transform & transform;
        std::shared_ptr<Texture> texture;

    protected:

        TextureOffset toNormalised(TextureOffset to)
        {
            glm::vec3 whc = texture->size(); 
            if (to.lx < 0.0){ to.lx = whc.x; }
            if (to.ly < 0.0){ to.ly = whc.y; }
            return TextureOffset(
                std::clamp(to.tx / whc.x, 0.0f, 1.0f),
                std::clamp(to.ty / whc.y, 0.0f, 1.0f),
                std::clamp(to.lx / whc.x, 0.0f, 1.0f),
                std::clamp(to.ly / whc.y, 0.0f, 1.0f)
            );
        }

        TextureOffset fromNormalised(TextureOffset to) const
        {
            glm::vec3 whc = texture->size();
            return TextureOffset(
                to.tx * whc.x,
                to.ty * whc.y,
                to.lx * whc.x,
                to.ly * whc.y
            );
        }

        float alpha;
        TextureOffset texOffset;
    
    };
}

#endif /* SPRITE_H */
