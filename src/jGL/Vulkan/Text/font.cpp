#include <jGL/Vulkan/Text/font.h>

namespace jGL::Vulkan
{
    Font::Font(const Device & device, const Command & command, uint8_t w)
    : width(w)
    {
        FT_Library ftLib;

        if (FT_Init_FreeType(&ftLib))
        {
        }

        FT_Face ftFace;

        if (FT_New_Memory_Face(ftLib,DefaultFont,sizeof(DefaultFont),0,&ftFace))
        {
        }

        FT_Set_Pixel_Sizes(ftFace,0,w);

        for (unsigned char ch = 0; ch < 128; ch++)
        {
            glyphs[ch] = Glyph(device, command, ftFace, ch);
        }

        FT_Done_Face(ftFace);
        FT_Done_FreeType(ftLib);
    }
}