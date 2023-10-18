#ifndef GLYPH
#define GLYPH

#include <ft2build.h>
#include FT_FREETYPE_H

#include <jGL/defaultFont.h>

#include <jGL/Vulkan/Device/device.h>
#include <jGL/Vulkan/Command/command.h>
#include <jGL/Vulkan/Texture/vkTexture.h>
#include <jGL/Vulkan/Texture/vkSampler.h>

#include <glm/glm.hpp>

#include <string>
#include <memory>

#include <iostream>


namespace jGL::Vulkan
{
    class Glyph
    {
    
    public:

        Glyph() = default;

        Glyph
        (
            const Device & device, 
            const Command & command, 
            const FT_Face & face, 
            unsigned char ch
        );

    private:

        std::unique_ptr<vkTexture> bitmap;
        unsigned char character;
        glm::ivec2 size, bearing;
        uint64_t offset;

    };
}

#endif /* GLYPH */
