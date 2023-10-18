#ifndef TEXTURE
#define TEXTURE

#include <vendored/stb_image.h>

#include <string>

namespace jGL
{

    class Texture
    {

    public:

        enum class Type {RGB, RGBA};

        Texture() = default;

        virtual void bind(unsigned unit) = 0;

    protected:

        int width, height, channels;

    };
}

#endif /* TEXTURE */
