#ifndef FONT
#define FONT

#include <jGL/Vulkan/Text/glyph.h>

namespace jGL::Vulkan
{
    class Font 
    {

    public:

        Font(const Device & device, const Command & command, uint8_t w);

        const VkImageView & getGlyphView(unsigned char c) { return glyphs[c].getVkImageView(); }

        std::array<glm::vec4, 6> getGlyphVertices(float x, float y, float scale, unsigned char c) { return glyphs[c].vertices(x,y,scale);}

    private:

        uint8_t width;
        std::map<unsigned char, Glyph> glyphs;

    };
}

#endif /* FONT */
