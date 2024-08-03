#ifndef SHAPERENDERER_H
#define SHAPERENDERER_H

#include <jGL/shape.h>
#include <jGL/shader.h>

#include <unordered_map>
#include <map>
#include <string>
#include <iterator>
#include <vector>
#include <algorithm>

namespace jGL
{

    /** 
     * @brief User name for a Shape.
     * @typedef SpriteId 
     * */
    typedef std::string ShapeId;

    /**
     * @brief Renders shapes with optional rendering priority.
     * @remark Currently Circle and Rectangle are defined as shaders.
     */
    class ShapeRenderer
    {

    public:
    
        /**
         * @brief Construct a new ShapeRenderer
         * 
         * @param sizeHint Hint at the number of shapes
         */
        ShapeRenderer(size_t sizeHint = 8)
        {
            shapes.reserve(sizeHint); 
        }

        std::shared_ptr<Shape> getShape(ShapeId id);

        const Transform & getTransform(ShapeId id) { return getShape(id)->transform; }
        const glm::vec4 & getColour(ShapeId id) { return getShape(id)->colour; }

        /**
         * @brief Draw with overriding render priority and shader.
         * 
         * @param shader A Shader to draw all the Sprites with.
         * @param ids Render priorities for the Sprites.
         */
        virtual void draw(std::shared_ptr<Shader> shader, std::multimap<RenderPriority, ShapeId> ids) = 0;

        /**
         * @brief Draw with overriding render priority.
         * 
         * @param ids Render priorities for the Sprites.
         */
        virtual void draw(std::multimap<RenderPriority, ShapeId> ids) = 0;
        
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
        
        virtual void add(std::shared_ptr<Shape> s, ShapeId id, RenderPriority priority = 0);
        
        virtual void remove(ShapeId id)
        {
            if (shapes.find(id) != shapes.end())
            {
                shapes.erase(id);
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

        virtual void clear() { ids.clear(); shapes.clear(); }

        bool hasId(const ShapeId id) const { return shapes.find(id) != shapes.end(); } 

        virtual void setProjection(glm::mat4 p) {projection = p;}

        virtual void updatePriority(ShapeId id, RenderPriority newPriority)
        {
            if (shapes.find(id) == shapes.end()){ return; }

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

        std::unordered_map<ShapeId, std::shared_ptr<Shape>> shapes;

        std::multimap<RenderPriority, ShapeId> ids;

        glm::mat4 projection = glm::mat4(0.0f);

    };
}

#endif /* SHAPERENDERER_H */
