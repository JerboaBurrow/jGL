#include <jGL/shapeRenderer.h>

namespace jGL
{
    // std::shared_ptr<Shape> ShapeRenderer::getShape(ShapeId id)
    // {
    //     return shapes[id].first;
    // }

    // void ShapeRenderer::add(std::shared_ptr<Shape> s, ShapeId id, RenderPriority priority)
    // {
    //     if (shapes.find(id) != shapes.end())
    //     {
    //         throw std::runtime_error("id: "+id+", already use in ShapeRenderer");
    //     }

    //     shapes[id] = std::pair(s, priority);
    //     auto pos = std::upper_bound
    //     (
    //         shapes_vectorised.begin(),
    //         shapes_vectorised.end(),
    //         std::pair(RenderInfo(id, priority), nullptr),
    //         []
    //         (
    //             std::pair<RenderInfo, std::shared_ptr<Shape>> l,
    //             std::pair<RenderInfo, std::shared_ptr<Shape>> r
    //         )
    //         {
    //             return l.first.priority < r.first.priority;
    //         }
    //     );
    //     shapes_vectorised.insert(pos, std::pair(RenderInfo(id, priority), s));
    // }

    // void ShapeRenderer::remove(ShapeId id)
    // {
    //     if (shapes.find(id) == shapes.end()) { return; }

    //     auto shape = shapes[id];
    //     shapes.erase(id);

    //     auto pos = std::find_if
    //     (
    //         shapes_vectorised.begin(),
    //         shapes_vectorised.end(),
    //         [id](std::pair<RenderInfo, std::shared_ptr<Shape>> v)
    //         {
    //             return v.first.id == id;
    //         }
    //     );
    //     if (pos != shapes_vectorised.end())
    //     {
    //         shapes_vectorised.erase(pos);
    //     }
    // }
}