#include <jGL/OpenGL/Text/glFont.h>

namespace jGL::GL
{
    void glFont::upload(uint16_t w, uint16_t h)
    {
        fontBitmap = std::make_shared<glTexture2DByte>
        (
            bitmapPixels,
            w,
            h
        );
    }
}
