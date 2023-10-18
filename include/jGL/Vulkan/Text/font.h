#ifndef FONT
#define FONT

#include <jGL/Vulkan/Text/glyph.h>

namespace jGL::Vulkan
{
    class Font 
    {

    public:

        Font(const Device & device, const Command & command, uint8_t w);

    private:

        uint8_t width;
        std::map<unsigned char, Glyph> glyphs;

    };
}

#endif /* FONT */
