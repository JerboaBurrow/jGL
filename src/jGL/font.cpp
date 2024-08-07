#include <jGL/font.h>

namespace jGL
{
    Font::Font(uint8_t w)
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

        int s = 0;

        for (unsigned char ch = 0; ch < 128; ch++)
        {
            glyphs[ch] = Glyph(ftFace, ch);
            s = std::max(s, std::max(glyphs[ch].getSize().x, glyphs[ch].getSize().y));
        }

        int chWidth = s;
        int chHeight = s;

        bitmapPixels = std::vector<std::byte>(chWidth*chHeight*128);

        uint16_t cursorX = 0;
        uint16_t cursorY = 0;
        const uint16_t width = chWidth*16;

        // we place characters to 128 in a bitmap of 16 characters in width

        /*  
             ________________________________________
            |    |    | <-- cx,cy, blocks coords     |
            |  A |  B |  . . .                       |
            |____|___<-- px,py, pixels in a block    |
            |                                        |
            |    .                                   |
            |    .                                   |
            |    .                                   |
            |                                        |
            |________________________________________|
        */

        for (unsigned char ch = 0; ch < 128; ch++)
        {
            std::vector<unsigned char> chPixels = glyphs[ch].getPixels();

            int gw = glyphs[ch].getSize().x;
            int gh = glyphs[ch].getSize().y;

            uint16_t px = cursorX;
            uint16_t py = cursorY;

            glyphOffset[ch] = glm::vec4(cursorX*chWidth+(int(ch)%16), cursorY, gw, gh);

            for (int i = 0; i < gw*gh; i++)
            {
                if (i > 0 && i % gw == 0)
                {
                    px = cursorX;
                    py += 1;
                }
                bitmapPixels[py*width+px+cursorX*chWidth] = std::byte(chPixels[i]);
                px++;
            }

            cursorX += 1;
            
            if (cursorX >= 16)
            {
                cursorX = 0;
                cursorY += chHeight;
            }
        }

        for (unsigned char ch = 0; ch < 128; ch++)
        {
            glyphOffset[ch] = glm::vec4
            (
                glyphOffset[ch].x / width,
                glyphOffset[ch].y / cursorY,
                glyphOffset[ch].z / width,
                glyphOffset[ch].w / cursorY
            );
        }

        bw = width;
        bh = cursorY;

        FT_Done_Face(ftFace);
        FT_Done_FreeType(ftLib);
    }

    std::array<glm::vec4, 6> Font::getGlyphVertices(float x, float y, float scale, unsigned char ch)
    {
        std::array<glm::vec4, 6> vertices;

        std::array<glm::vec2, 6> pos = glyphs[ch].vertices(x,y,scale);
        glm::vec4 offset = glyphOffset[ch];

        vertices[0] = {pos[0].x, pos[0].y, offset.x,            offset.y};
        vertices[1] = {pos[1].x, pos[1].y, offset.x,            offset.y+offset.w};
        vertices[2] = {pos[2].x, pos[2].y, offset.x+offset.z,   offset.y+offset.w};
        vertices[3] = {pos[3].x, pos[3].y, offset.x,            offset.y};
        vertices[4] = {pos[4].x, pos[4].y, offset.x+offset.z,   offset.y+offset.w};
        vertices[5] = {pos[5].x, pos[5].y, offset.x+offset.z,   offset.y};

        return vertices;

    }
}