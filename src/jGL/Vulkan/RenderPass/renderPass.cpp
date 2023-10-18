#include <jGL/Vulkan/RenderPass/renderPass.h>

namespace jGL::Vulkan
{

    RenderPass::RenderPass
    (
        const Device & device, 
        VkSampleCountFlagBits msaaSamples,
        VkFormat swapChainImageFormat
    )
    {
        VkAttachmentDescription colour{};
        colour.format = swapChainImageFormat;
        colour.samples = msaaSamples;
        // do before rendering; clear
        colour.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        // do after rendering, store
        colour.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        // not using stencil data; don't care
        colour.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colour.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        // we clear image so don't need to worry about intial layout
        colour.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        // image is auto converted to presentation layout
        // since msaa is enabled optimal is needed as they
        // cannot be presented as is
        colour.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colourRef{};
        // e.g. layout(location = 0) out vec4 outColour
        colourRef.attachment = 0;
        colourRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // resolve attachment for msaa
        VkAttachmentDescription colourResolve{};
        colourResolve.format = swapChainImageFormat;
        colourResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colourResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colourResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colourResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colourResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colourResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colourResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // resolve ref
        VkAttachmentReference colourResolveRef{};
        colourResolveRef.attachment = 1;
        colourResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // one subpass

        VkSubpassDescription subpass{};
        // specify as a graphics subpass
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colourRef;
        subpass.pResolveAttachments = &colourResolveRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        // wait on the colour attachment output stage
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2>  attachments = {colour, colourResolve};

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(device.getVkDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass");
        }
    }

};