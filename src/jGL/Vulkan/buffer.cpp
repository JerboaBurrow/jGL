#include <jGL/Vulkan/buffer.h>

namespace jGL::Vulkan
{

    void Buffer::copy(const Buffer & src, const Command & command)
    {
        copyBuffer(src.getVkBuffer(), buffer, size, command);
    }

    void Buffer::copyMemory
    (
        const VkDeviceSize offset,
        const VkDeviceSize size,
        void * data,
        const VkMemoryMapFlags flags
    )
    {
        void* v;
        vkMapMemory(device.getVkDevice(), memory, offset, size, flags, &v);
            memcpy(v, data, static_cast<size_t>(size));
        vkUnmapMemory(device.getVkDevice(), memory);
    }

    uint32_t findMemoryType
    (
        const Device & device,
        uint32_t typeFilter, 
        VkMemoryPropertyFlags properties
    ) 
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(device.getVkPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type");
    }


    void Buffer::createBuffer
    (
        VkDeviceSize size, 
        VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags properties
    ) 
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device.getVkDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vertex buffer");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device.getVkDevice(), buffer, &memRequirements);

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
            throw std::runtime_error("Failed to allocate vertex buffer memory");
        }

        // last is an offset in memory
        vkBindBufferMemory(device.getVkDevice(), buffer, memory, 0);
    }

    void Buffer::copyBuffer
    (
        VkBuffer src, 
        VkBuffer dst, 
        VkDeviceSize size,
        const Command & command
    )
    {
        VkCommandBuffer commandBuffer = command.beginCommand(device);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

        command.endCommand(device, commandBuffer);
    }
}