#include <jGL/Vulkan/Texture/vkTexture.h>

namespace jGL::Vulkan
{

    vkTexture::vkTexture(const Device & device)
    : device(device)
    {}

    vkTexture::vkTexture
    (
        const Device & device,
        const Command & command,
        std::string imageFile, 
        Texture::Type type
    )
    : device(device)
    {

        int width, height, channels;
        stbi_uc * pixels;

        if (type == Texture::Type::RGB)
        {
            pixels = stbi_load(imageFile.c_str(), &width, &height, &channels, STBI_rgb);
            this->channels = 3;
            format = VK_FORMAT_R8G8B8_SRGB;
        }
        else if (type == Texture::Type::RGBA)
        {
            pixels = stbi_load(imageFile.c_str(), &width, &height, &channels, STBI_rgb_alpha);
            this->channels = 4;
            format = VK_FORMAT_R8G8B8A8_SRGB;         
        }

        this->width = width;
        this->height = height;

        uploadImage
        (
            command,
            std::vector<unsigned char> {pixels, pixels+width*height*channels},
            format,
            this->width,
            this->height,
            this->channels
        );

        if (!pixels)
        {
            throw std::runtime_error("Failed to load image "+imageFile);
        }

        stbi_image_free(pixels);

    }

    vkTexture::vkTexture
    (
        const Device & device,
        const Command & command,
        uint32_t width, 
        uint32_t height, 
        uint32_t channels, 
        VkFormat format, 
        std::vector<unsigned char> pixels
    )
    : device(device)
    {
        this->format = format;
        this->width = width;
        this->height = height;
        this->channels = channels;

        uploadImage
        (
            command,
            pixels,
            format,
            this->width,
            this->height,
            this->channels
        );
    }

    vkTexture::vkTexture
    (
        const Device & device,
        const Command & command,
        uint32_t width, 
        uint32_t height, 
        uint32_t channels, 
        VkFormat format
    )
    : vkTexture
      (
        device,
        command,
        width, 
        height, 
        channels, 
        format,
        std::vector<unsigned char>(width*height*channels, 0)
      )
    {}

    void createImage
    (
        const Device & device,
        uint32_t width, 
        uint32_t height,
        uint32_t mipLevels, 
        VkSampleCountFlagBits numSamples,
        VkFormat format, 
        VkImageTiling tiling, 
        VkImageUsageFlags usage, 
        VkMemoryPropertyFlags properties,
        VkImage & image,
        VkDeviceMemory & memory
    )
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device.getVkDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create image");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device.getVkDevice(), image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType
        (
            device,
            memRequirements.memoryTypeBits, 
            properties
        );

        if (vkAllocateMemory(device.getVkDevice(), &allocInfo, nullptr, &memory) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to allocate image memory");
        }

        vkBindImageMemory(device.getVkDevice(), image, memory, 0);
    }

    void createImageView
    (
        const Device & device,
        VkFormat format, 
        VkImageAspectFlags aspectFlags, 
        uint32_t mipLevels,
        VkImage & image,
        VkImageView & imageView
    )
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device.getVkDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create texture image view");
        }
    }

    void vkTexture::transitionImageLayout
    (
        const Command & command,
        VkFormat format, 
        VkImageLayout oldLayout, 
        VkImageLayout newLayout
    ) 
    {

        VkCommandBuffer commandBuffer = command.beginCommand(device);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else 
        {
            throw std::invalid_argument("unsupported layout transition");
        }

        vkCmdPipelineBarrier
        (
            commandBuffer,
            sourceStage, 
            destinationStage,
            0,
            0, 
            nullptr,
            0, 
            nullptr,
            1, 
            &barrier
        );

        command.endCommand(device, commandBuffer);
    }

    void vkTexture::copyBufferToImage
    (
        const Command & command,
        Buffer & buffer, 
        uint32_t width, 
        uint32_t height
    ) 
    {

        VkCommandBuffer commandBuffer = command.beginCommand(device);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = 
        {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage
        (
            commandBuffer,
            buffer.getVkBuffer(),
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        command.endCommand(device, commandBuffer);
    }

    void vkTexture::uploadImage
    (
        const Command & command,
        std::vector<unsigned char> pixels,
        VkFormat format,
        uint32_t width,
        uint32_t height,
        uint32_t channels
    )
    {
        VkDeviceSize imageSize = width*height*channels;

        Buffer stagingBuffer
        (
            device,
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        stagingBuffer.copyMemory
        (
            0,
            imageSize,
            pixels.data()
        );

        createImage
        (
            device,
            width, 
            height,
            1,
            VK_SAMPLE_COUNT_1_BIT,
            format, 
            VK_IMAGE_TILING_OPTIMAL, 
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            image,
            imageMemory
        );
        

        transitionImageLayout
        (
            command,
            format, 
            VK_IMAGE_LAYOUT_UNDEFINED, 
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        );
        
        copyBufferToImage
        (
            command,
            stagingBuffer, 
            static_cast<uint32_t>(width), 
            static_cast<uint32_t>(height)
        );
        
        transitionImageLayout
        (
            command,
            format, 
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );

        createImageView(device, format, VK_IMAGE_ASPECT_COLOR_BIT, 1, image, imageView);
    }

    void vkTexture::replace
    (
        const Command & command,
        uint32_t width, 
        uint32_t height, 
        uint32_t channels, 
        VkFormat format, 
        std::vector<unsigned char> pixels
    )
    {
        if 
        (
            this->width == width && 
            this->height == height && 
            this->channels == channels && 
            this->format == format &&
            pixels.size() == width*height*channels
        )
        {
            VkDeviceSize imageSize = width*height*channels;

            Buffer stagingBuffer
            (
                device,
                imageSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );

            stagingBuffer.copyMemory
            (
                0,
                imageSize,
                pixels.data()
            );

            transitionImageLayout
            (
                command,
                format, 
                VK_IMAGE_LAYOUT_UNDEFINED, 
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
            );
            
            copyBufferToImage
            (
                command,
                stagingBuffer, 
                static_cast<uint32_t>(width), 
                static_cast<uint32_t>(height)
            );

            transitionImageLayout
            (
                command,
                format, 
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            );

        }
        else
        {
            vkDestroyImageView(device.getVkDevice(), imageView, nullptr);
            vkDestroyImage(device.getVkDevice(), image, nullptr);
            vkFreeMemory(device.getVkDevice(), imageMemory, nullptr);

            this->format = format;
            this->width = width;
            this->height = height;
            this->channels = channels;

            uploadImage
            (
                command,
                pixels,
                format,
                width,
                height,
                channels
            );
        }
    }
}