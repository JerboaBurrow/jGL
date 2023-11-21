#include <jGL/Vulkan/Text/glyph.h>

namespace jGL::Vulkan
{
    Glyph::Glyph
    (
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

        bitmap = std::vector<unsigned char>(width*height, 0);

        unsigned j = 0;
        for (unsigned i = 0; i < width*height; i++)
        {
            bitmap[i] = pixels[i];
        }
   
        size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        offset = face->glyph->advance.x;
    }
}