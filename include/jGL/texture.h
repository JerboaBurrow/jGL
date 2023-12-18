#ifndef TEXTURE
#define TEXTURE

#include <glm/glm.hpp>

#include <vendored/stb_image.h>

#include <id.h>

#include <string>

namespace jGL
{

    class Texture
    {

    public:

        enum class Type {RGB, RGBA};

        Texture()
        : width(0), height(0), channels(0), id(Id::next())
        {}

        virtual void bind(unsigned unit) = 0;

        bool operator==(const Texture & rhs) const { return this->id == rhs.id; }

        const Id & getId() const { return id; }

        const glm::ivec3 size() const { return glm::ivec3(width, height, channels); }

    protected:

        int width, height, channels;

        Id id;

    };

}

#endif /* TEXTURE */
