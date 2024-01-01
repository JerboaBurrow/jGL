#include <jGL/Vulkan/swapchain.h>

namespace jGL::Vulkan
{
    void Swapchain::createSwapchain(unsigned width, unsigned height, const Device & device,  const VkSurfaceKHR & surface)
    {
        jLog::Log log;
        std::stringstream message;

        SwapchainSupportDetails swapchainSupport = device.getPhysicalDevice().getSwapchainSupportDetails();

        VkSurfaceFormatKHR surfaceFormat = chooseSwapchainSurfaceFormat(swapchainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapchainPresentMode(swapchainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(width, height, swapchainSupport.capabilities);

        uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

        message << "[VULKAN] creating Swapchain with " << imageCount << " images\n";
        jLog::INFO(message.str()) >> log;

        if 
        (
            swapchainSupport.capabilities.maxImageCount > 0 && 
            imageCount > swapchainSupport.capabilities.maxImageCount
        )
        {
            imageCount = swapchainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo {};

        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        // 1 unless sterescopic 3D
        createInfo.imageArrayLayers = 1;
        // for direct rendering, VK_IMAGE_USAGE_TRANSFER_DST_BIT would be 
        // for saving images for post-processing
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = device.getPhysicalDevice().getQueueFamilyIndices();
        uint32_t queueFamilyIndices[] = 
        {
            indices.graphicsFamily.value(), 
            indices.presentFamily.value()
        };

        if (indices.graphicsFamily != indices.presentFamily) 
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } 
        else 
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        // false if want to get pixels obscured
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device.getVkDevice(), &createInfo, nullptr, &swapchain) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create swap chain");
        }

        vkGetSwapchainImagesKHR(device.getVkDevice(), swapchain, &imageCount, nullptr);
        swapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device.getVkDevice(), swapchain, &imageCount, swapchainImages.data());
    
        swapchainImageFormat = surfaceFormat.format;
        swapchainExtent = extent;

        createImageViews(device);
    }

    void Swapchain::recreateSwapchain(unsigned width, unsigned height, const Device & device, const VkSurfaceKHR & surface)
    {
        vkDeviceWaitIdle(device.getVkDevice());

        cleanupSwapchain(device);

        createSwapchain(width, height, device, surface);
        createImageViews(device);
    }

    void Swapchain::cleanupSwapchain(const Device & device)
    {
        for (auto imageView : swapchainImageViews)
        {
            vkDestroyImageView(device.getVkDevice(), imageView, nullptr);
        }

        vkDestroySwapchainKHR(device.getVkDevice(), swapchain, nullptr);
    }

    VkSurfaceFormatKHR Swapchain::chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & availableFormats)
    {
        // selecting colour depth
        // VK_FORMAT_B8G8R8A8_SRGB = store B, G, R, alpha channels in that order, 8 bit unsigned integer, totaling 32 bits per pixel.
        for (const auto & availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        // could rank, here just pick first
        return availableFormats[0];
    }

    VkPresentModeKHR Swapchain::chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> & availablePresentModes)
    {
        /*

            VK_PRESENT_MODE_IMMEDIATE_KHR: Images submitted by your application 
                are transferred to the screen right away, which may result 
                in tearing.

            VK_PRESENT_MODE_FIFO_KHR: The swap chain is a queue where the display
                takes an image from the front of the queue when the display is 
                refreshed and the program inserts rendered images at the back 
                of the queue. If the queue is full then the program has to wait. 
                This is most similar to vertical sync as found in modern games. 
                The moment that the display is refreshed is known as 
                "vertical blank".

            VK_PRESENT_MODE_FIFO_RELAXED_KHR: This mode only differs from the 
                previous one if the application is late and the queue was 
                empty at the last vertical blank. Instead of waiting for the 
                next vertical blank, the image is transferred right away when 
                it finally arrives. This may result in visible tearing.

            VK_PRESENT_MODE_MAILBOX_KHR: This is another variation of the 
                second mode. Instead of blocking the application when the queue 
                is full, the images that are already queued are simply replaced 
                with the newer ones. This mode can be used to render frames as 
                fast as possible while still avoiding tearing, resulting in fewer 
                latency issues than standard vertical sync. This is commonly known 
                as "triple buffering", although the existence of three buffers 
                alone does not necessarily mean that the framerate is unlocked.


            Only the VK_PRESENT_MODE_FIFO_KHR mode is guaranteed to be available

        */

        for (const auto & availablePresentMode: availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }
       
       return VK_PRESENT_MODE_FIFO_KHR;

    }

    VkExtent2D Swapchain::chooseSwapExtent(unsigned width, unsigned height, const VkSurfaceCapabilitiesKHR & capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            // window manager allow different windows res to swap chain image res
            return capabilities.currentExtent;
        }
        else 
        {
            VkExtent2D actualExtent = 
            {
                width, height
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void Swapchain::createImageViews(const Device & device)
    {
        swapchainImageViews.resize(swapchainImages.size());
        for(size_t i = 0; i < swapchainImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapchainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapchainImageFormat;
            // map components to themselves
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
            // layers can be used for 3d images
            createInfo.subresourceRange.layerCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device.getVkDevice(), &createInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create image view");
            }
        }
    }
}