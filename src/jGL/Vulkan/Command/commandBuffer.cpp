#include <jGL/Vulkan/Command/commandBuffer.h>

namespace jGL::Vulkan
{
    CommandBuffer::CommandBuffer(const Device & device, const CommandPool & commandPool, const unsigned concurrentFrames)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool.getVkCommandPool();
        // cannot be called from other command buffers, only submitted
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = concurrentFrames;

        commandBuffers.resize(concurrentFrames);

        if (vkAllocateCommandBuffers(device.getVkDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffers");
        }

    }
}