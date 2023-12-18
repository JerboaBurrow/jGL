#include <jGL/OpenGL/Text/font.h>

namespace jGL::GL
{
    void glFont::upload(uint16_t w, uint16_t h)
    {
        fontBitmap = std::make_shared<glTexture2DByte>
        (
            bitmapPixels.data(),
            w,
            h
        );
    }
}
