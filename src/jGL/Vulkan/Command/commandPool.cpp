#include <jGL/Vulkan/Command/commandPool.h>

namespace jGL::Vulkan
{
    CommandPool::CommandPool(const Device & device)
    {
        QueueFamilyIndices queueFamilyIndices = device.getPhysicalDevice().getQueueFamilyIndices();

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        // command buffers can be rerecorded individually
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(device.getVkDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to creaate command pool");
        }
    }
}