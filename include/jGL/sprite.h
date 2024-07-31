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
 * Rendered using the TextureRegion (pixel units) region of the Texture, at the alpha value.
 * 
 */

namespace jGL
{
    class Sprite
    {

    public:

        /**
         * @brief Construct a Sprite
         *
         * @param tra x, y, theta, scale Transform
         * @param to Region of texture to draw
         * @param tex Texture to draw
         * @param alpha Transparency modifier (alpha * texture alpha)
         */
        Sprite
        ( 
            const Transform & tra,
            TextureRegion to,
            const std::shared_ptr<Texture> tex,
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
         * @brief Get the TextureRegion in pixel units.
         * 
         * @return TextureRegion
         */
        TextureRegion getTextureRegion() const
        { 
            return fromNormalised(texOffset);
        }

        /**
         * @brief Get the NormalisedTextureRegion in normalised units.
         *
         * @return TextureRegion
         */
        NormalisedTextureRegion getNormalisedTextureRegion() const
        {
            return texOffset;
        }

        /**
         * @brief Set the Texture Offset from a pixel region.
         * 
         * @remark Values outside of the textures pixel range will be clamped.
         * @remark Lengths lx or ly < 0 will be set to maximum.
         * 
         */
        void setTextureRegion(TextureRegion to) { texOffset = toNormalised(to); }

        const Transform & transform;
        const std::shared_ptr<Texture> texture;

    protected:

        NormalisedTextureRegion toNormalised(TextureRegion to)
        {
            glm::vec3 whc = texture->size(); 
            if (to.lx == 0){ to.lx = whc.x; }
            if (to.ly == 0){ to.ly = whc.y; }
            return NormalisedTextureRegion(
                std::clamp(float(to.tx) / float(whc.x), 0.0f, 1.0f),
                std::clamp(float(to.ty) / float(whc.y), 0.0f, 1.0f),
                std::clamp(float(to.lx) / float(whc.x), 0.0f, 1.0f),
                std::clamp(float(to.ly) / float(whc.y), 0.0f, 1.0f)
            );
        }

        TextureRegion fromNormalised(NormalisedTextureRegion to) const
        {
            glm::vec3 whc = texture->size();
            return TextureRegion(
                uint16_t(to.tx * whc.x),
                uint16_t(to.ty * whc.y),
                uint16_t(to.lx * whc.x),
                uint16_t(to.ly * whc.y)
            );
        }

        float alpha;
        NormalisedTextureRegion texOffset;
    
    };
}

#endif /* SPRITE_H */
