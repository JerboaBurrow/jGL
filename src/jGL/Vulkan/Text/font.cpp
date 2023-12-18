#include <jGL/Vulkan/Text/font.h>

namespace jGL::Vulkan
{
    void vkFont::upload(uint16_t w, uint16_t h)
    {
        fontBitmap = std::make_shared<vkTexture>
        (
            device, 
            command, 
            w,
            h, 
            1, 
            VK_FORMAT_R8_UINT, 
            bitmapPixels
        );

        view = std::static_pointer_cast<vkTexture>(fontBitmap)->getVkImageView();
    }
}