#ifndef FONT
#define FONT

#include <jGL/Vulkan/Text/glyph.h>
#include <jGL/Vulkan/Device/device.h>
#include <jGL/Vulkan/Command/command.h>
#include <jGL/Vulkan/Texture/vkTexture.h>
#include <jGL/Vulkan/Texture/vkSampler.h>

#include <algorithm>


namespace jGL::Vulkan
{
    class Font 
    {

    public:

        Font(const Device & device, const Command & command, uint8_t w);

        const VkImageView & getGlyphView() { return fontBitmap->getVkImageView(); }
        const glm::ivec2 getBitmapSize() const { return glm::ivec2(fontBitmap->size().x, fontBitmap->size().y); }

        std::array<glm::vec4, 6> getGlyphVertices(float x, float y, float scale, unsigned char c);
        glm::vec4 getGlyphOffset(unsigned char c) { return glyphOffset[c]; }
        glm::ivec2 getGlyphSize(unsigned char c) { return glyphs[c].getSize(); }

        float spacing(float scale) { return scale * width; }

    private:

        uint8_t width;

        std::map<unsigned char, Glyph> glyphs;
        std::map<unsigned char, glm::vec4> glyphOffset;

        std::unique_ptr<vkTexture> fontBitmap;

    };
}

#endif /* FONT */
