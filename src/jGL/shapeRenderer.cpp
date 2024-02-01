#include <jGL/shapeRenderer.h>

namespace jGL
{
    std::shared_ptr<Shape> ShapeRenderer::getShape(ShapeId id)
    {
        return shapes[id];
    }

    void ShapeRenderer::add(std::shared_ptr<Shape>  s, ShapeId id, uint16_t priority)
    {
        
        if (shapes.find(id) != shapes.end())
        {
            throw std::runtime_error("id: "+id+", already use in ShapeRenderer");
        }

        shapes[id] = s;
        ids.insert(std::pair(priority, id));
    }
}