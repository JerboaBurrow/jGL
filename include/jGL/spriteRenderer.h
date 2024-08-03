#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H

#include <jGL/sprite.h>
#include <jGL/shader.h>

#include <unordered_map>
#include <string>
#include <iterator>
#include <vector>
#include <algorithm>

namespace jGL
{
    /** 
     * @brief User name for a Sprite.
     * @typedef SpriteId 
     * */
    typedef std::string SpriteId;

    /**
     * @brief Renders sprites in batches, with optional render priority.
     * 
     * @remark Currently there are 4 texture slots loaded at once.
     * @remark Keeping to 4 textures is most efficient (1 draw call), atlas textures are useful for this.
     * @remark RenderPriority of Sprites may lead to inefficient batching across textures. 
     * Try to keep similar RenderPriority within the same texture/ group of 4 textures.
     */
    class SpriteRenderer
    {

    public:

        /**
         * @brief Largest number of concurrent textures bound for one batch.
         * 
         */
        static const uint8_t MAX_BATCH_BOUND_TEXTURES = MAX_SPRITE_BATCH_BOUND_TEXTURES;
    
        /**
         * @brief Construct a new SpriteRenderer.
         * 
         * @param sizeHint Reserve some memory for this many Sprites.
         */
        SpriteRenderer(size_t sizeHint = 8)
        {
            sprites.reserve(sizeHint); 
            textures.reserve(MAX_BATCH_BOUND_TEXTURES);
        }

        Sprite & getSprite(SpriteId id);

        const Transform & getTransform(SpriteId id) { return getSprite(id).transform; }

        /**
         * @brief Get a Sprites TextureRegion
         * @remark In pixel units, see TextureRegion::getTextureRegion()
         * 
         * @param id 
         * @return const TextureRegion
         */
        const TextureRegion getTextureRegion(SpriteId id) { return getSprite(id).getTextureRegion(); }

        /**
         * @brief Draw with overriding render priority and shader.
         * 
         * @param shader An OpenGL Shader to draw all the Sprites with.
         * @param ids Render priorities for the Sprites.
         */
        virtual void draw(std::shared_ptr<Shader> shader, std::multimap<RenderPriority, SpriteId> ids) = 0;

        /**
         * @brief Draw with overriding render priority.
         * 
         * @param ids Render priorities for the Sprites.
         */
        virtual void draw(std::multimap<RenderPriority, SpriteId> ids) = 0;
        
        /**
         * @brief Draw with overriding shader.
         * 
         * @param shader An Shader to draw all the Sprites with.
         */
        virtual void draw(std::shared_ptr<Shader> shader) { draw(shader, ids); }

        /**
         * @brief Draw with default shader and priority.
         * 
         */
        virtual void draw() { draw(ids); }

        virtual void add(Sprite s, SpriteId id, RenderPriority priority = 0);

        virtual void remove(SpriteId id)
        {
            if (sprites.find(id) != sprites.end())
            {
                sprites.erase(id);
            }

            for (auto & e : ids)
            {
                if (e.second == id)
                {
                    ids.erase(e.first);
                    break;
                }
            }
        }

        bool hasId(const SpriteId id) const { return sprites.find(id) != sprites.end(); } 

        virtual void clear() { ids.clear(); sprites.clear(); }

        virtual void setProjection(glm::mat4 p) {projection = p;}

        virtual void updatePriority(SpriteId id, RenderPriority newPriority)
        {
            if (sprites.find(id) == sprites.end()){ return; }

            for (auto & e : ids)
            {
                if (e.second == id)
                {
                    ids.erase(e.first);
                    ids.insert(std::pair(newPriority, e.second));
                    break;
                }
            }
        }

    protected:

        std::vector<std::shared_ptr<Texture>> textures;

        std::unordered_map<SpriteId, Sprite> sprites;
        std::unordered_map<SpriteId, size_t> spriteToTexture;

        std::multimap<RenderPriority, SpriteId> ids;

        glm::mat4 projection = glm::mat4(0.0f);

    };
}

#endif /* SPRITERENDERER_H */
