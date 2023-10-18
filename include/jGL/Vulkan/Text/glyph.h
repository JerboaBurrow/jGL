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

        const VkImageView & getVkImageView() { return bitmap->getVkImageView(); }

        std::array<glm::vec4, 6> vertices(float x, float y, float scale)
        {
            float xpos = x + bearing.x * scale;
            float ypos = y - (size.y - bearing.y) * scale;

            float w = size.x * scale;
            float h = size.y * scale;

            std::array<glm::vec4, 6> v;
            v[0] = glm::vec4(xpos,   ypos+h, 0.0f, 0.0f);
            v[1] = glm::vec4(xpos,   ypos,   0.0f, 1.0f);
            v[2] = glm::vec4(xpos+w, ypos,   1.0f, 1.0f);
            v[3] = glm::vec4(xpos,   ypos+h, 0.0f, 0.0f);
            v[4] = glm::vec4(xpos+w, ypos,   1.0f, 1.0f);
            v[5] = glm::vec4(xpos+w, ypos+h, 1.0f, 0.0f);

            return v;
        }


    private:

        std::unique_ptr<vkTexture> bitmap;
        unsigned char character;
        glm::ivec2 size, bearing;
        uint64_t offset;

    };
}

#endif /* GLYPH */
