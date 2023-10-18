#include <jGL/Vulkan/framebuffer.h>

namespace jGL::Vulkan
{
    Framebuffer::Framebuffer
    (
        const Device & device, 
        const Swapchain & swapchain, 
        const RenderPass & renderPass,
        const VkImageView & msaaView,
        const VkImageView & resolveView
    )
    : device(device)
    {
    
        std::array<VkImageView,2> attachments = {msaaView, resolveView};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass.getVkRenderPass();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapchain.getVkExtend2D().width;
        framebufferInfo.height = swapchain.getVkExtend2D().height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device.getVkDevice(), &framebufferInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create framebuffer");
        }
    }
}