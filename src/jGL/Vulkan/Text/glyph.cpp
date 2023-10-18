#include <jGL/Vulkan/Text/glyph.h>

namespace jGL::Vulkan
{
    Glyph::Glyph
    (
        const Device & device,
        const Command & command, 
        const FT_Face & face, 
        unsigned char ch
    )
    : character(ch)
    {
        if (FT_Load_Char(face, ch, FT_LOAD_RENDER))
        {
            throw std::runtime_error("Failed to load glyph " + std::to_string(ch) + " from ASCII charset");
        }
        unsigned int width = face->glyph->bitmap.width;
        unsigned int height = face->glyph->bitmap.rows;
        unsigned char * pixels = face->glyph->bitmap.buffer;

        std::vector<unsigned char> vPixels(width*height, 0);

        for (unsigned i = 0; i < vPixels.size(); i++)
        {
            vPixels[i] = pixels[i];
        }

        if (vPixels.size() > 0)
        {
            bitmap = std::make_unique<vkTexture>
            (
                device, 
                command, 
                width, 
                height, 
                1, 
                VK_FORMAT_R8_UINT, 
                vPixels
            );
            
            size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
            bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
            offset = face->glyph->advance.x;
        }
    }
}