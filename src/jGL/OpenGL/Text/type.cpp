#include <jGL/OpenGL/Text/type.h>
#include <jGL/OpenGL/Text/typeUtils.h>

namespace jGL::GL
{

  Type::Type(std::string path, std::string font, uint8_t w)
  {
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    width = w;
    name = font;
    //construct freetype objects
    FT_Library ftLib;

    if (FT_Init_FreeType(&ftLib))
    {
      std::cout << "Could not init FreeType\n";
    }

    FT_Face ftFace;

    if (FT_New_Face(ftLib,(path+font).c_str(),0,&ftFace))
    {
      std::cout << "Could not load font: " + font + " at: " + path + "\n";
    }

    FT_Set_Pixel_Sizes(ftFace,0,width); //dynamic width for height 48

    glyphs.clear();

     glError("before load glyphs");

    loadASCIIGlyphs(ftFace,glyphs);

    FT_Done_Face(ftFace);
    FT_Done_FreeType(ftLib);

  }

  Type::Type(uint8_t w)
  {
    std::cout << "Using default font\n";
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    width = w;
    name = "Default";

    //construct freetype objects
    FT_Library ftLib;

    if (FT_Init_FreeType(&ftLib))
    {
      std::cout << "Could not init FreeType\n";
    }

    FT_Face ftFace;

    if (FT_New_Memory_Face(ftLib,DefaultFont,sizeof(DefaultFont),0,&ftFace))
    {
      std::cout << "Could not load font default font! Was Particles compiled correctly?\n";
    }

    FT_Set_Pixel_Sizes(ftFace,0,width); //dynamic width for height 48

    glyphs.clear();

     glError("before load glyphs");

    loadASCIIGlyphs(ftFace,glyphs);

    FT_Done_Face(ftFace);
    FT_Done_FreeType(ftLib);
  }

  Glyph & Type::operator[](char c)
  {
    return glyphs[c];
  }

}
