#include <jGL/Vulkan/vkDebug.h>

void vkError(VkResult result, std::string context)
{
    if (result != VK_SUCCESS)
    {
        context = string_VkResult(result) + std::string(" context: ") + context;
        throw std::runtime_error(context.c_str());
    }
}