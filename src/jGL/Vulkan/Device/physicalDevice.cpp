#include <jGL/Vulkan/Device/physicalDevice.h>

namespace jGL::Vulkan
{
    PhysicalDevice::PhysicalDevice(const VkInstance & instance, const VkSurfaceKHR & surface)
    {
        pick(instance, surface);
    }

    void PhysicalDevice::pick(const VkInstance & instance, const VkSurfaceKHR & surface)
    {
        jLog::Log log;
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0)
        {
            throw std::runtime_error("No Vulkan devices found");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        VkPhysicalDeviceProperties deviceProperties;

        for (const auto& physicalDevice : devices)
        {
            std::stringstream message;

            vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

            message << "[VULKAN] Found physicalDevice: " 
                      << deviceProperties.deviceName << " v"
                      << deviceProperties.driverVersion << "\n";

            if (isDeviceSuitable(physicalDevice, surface))
            {
                if (this->physicalDevice == VK_NULL_HANDLE)
                {
                    this->physicalDevice = physicalDevice;
                    this->queueFamilyIndices = findQueueFamilies(physicalDevice, surface);
                    this->msaaSamples = getMaxUsableSampleCount();
                    this->swapchainSupport = querySwapchainSupport(physicalDevice, surface);
                    this->deviceLimits = deviceProperties.limits;
                }
                else 
                {
                    alternatives.push_back
                    (
                        std::pair(deviceProperties.deviceName, physicalDevice)
                    );
                }

                msaaSamples = getMaxUsableSampleCount();
                message << "    Device supports \n" 
                        << "        " << msaaSamples << " msaa samples\n"
                        << "        " << deviceProperties.limits.maxSamplerAnisotropy << " anisotropy samples\n";
                jLog::INFO(message.str()) >> log;
            }
        }
    }

    bool PhysicalDevice::isDeviceSuitable(const VkPhysicalDevice physicalDevice,  const VkSurfaceKHR surface) const
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;

        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

        bool extensionsSupported = checkDeviceExtensionSupport(physicalDevice);

        bool swapchainAdequate = false;

        if (extensionsSupported)
        {
            SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice, surface);
            swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapchainAdequate && deviceFeatures.samplerAnisotropy;
    }

    QueueFamilyIndices PhysicalDevice::findQueueFamilies(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) const
    {
        QueueFamilyIndices indices;

        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(count);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilies.data());

        unsigned i = 0;
        for (const auto & queueFamily : queueFamilies)
        {
            VkBool32 presentSupport = false;
            // may be in different queues, could ask for both in one for
            // better performance and rate devices
            // https://vulkan-tutorial.com/en/Drawing_a_triangle/Presentation/Window_surface
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            
            if (presentSupport)
            {
                indices.presentFamily = i;
            }

            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }
            if (indices.isComplete())
            {
                break;
            }
            i++;
        }

        return indices;
    }

    bool PhysicalDevice::checkDeviceExtensionSupport(const VkPhysicalDevice physicalDevice) const
    {
        uint32_t extensionCount;

        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto & extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    SwapchainSupportDetails PhysicalDevice::querySwapchainSupport(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) const
    {
        SwapchainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
        }

        uint32_t presentationModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationModeCount, nullptr);

        if (presentationModeCount != 0)
        {
            details.presentModes.resize(presentationModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSampleCountFlagBits PhysicalDevice::getMaxUsableSampleCount() const
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;

        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts;

        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;
    }

}