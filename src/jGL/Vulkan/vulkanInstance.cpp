#include <jGL/Vulkan/vulkanInstance.h>

namespace jGL::Vulkan
{

  VulkanInstance::VulkanInstance(const Display & display)
  : jGLInstance(display)
  {

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "jGL::Vulkan";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, MAJOR, MINOR, PATCH);
    appInfo.pEngineName = "jGL";
    appInfo.engineVersion = VK_API_VERSION_1_0;
    appInfo.pNext = nullptr;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    // glfw 
    uint32_t glfwExtensionCount = 0;
    const char ** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    auto extensions = std::vector<const char *>(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkResult res = vkCreateInstance(&createInfo, nullptr, &instance);

    if (res != VK_SUCCESS)
    {
        std::string message = "Failed to create vulkan instance: ";
        switch (res)
        {
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                message += "VK_ERROR_OUT_OF_HOST_MEMORY";
                break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                message += "VK_ERROR_OUT_OF_DEVICE_MEMORY";
                break;
            case VK_ERROR_INITIALIZATION_FAILED:
                message += "VK_ERROR_INITIALIZATION_FAILED";
                break;
            case VK_ERROR_LAYER_NOT_PRESENT:
                message += "VK_ERROR_LAYER_NOT_PRESENT";
                break;
            case VK_ERROR_EXTENSION_NOT_PRESENT:
                message += "VK_ERROR_EXTENSION_NOT_PRESENT";
                break;
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                message += "VK_ERROR_INCOMPATIBLE_DRIVER";
                break;
            default:
                break;
        }
        throw std::runtime_error(message);
    }

    if (enableValidationLayers && !checkValidationLayerSupport())
    {
        throw std::runtime_error("Validation layers are not supported");
    }

    if (enableValidationLayers)
    {
        VkResult result;
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) 
        {
            result = func(instance, &debugCreateInfo, nullptr, &debugMessenger);
        } 
        else 
        {
            result = VK_ERROR_EXTENSION_NOT_PRESENT;
        }
        if(result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vulkan debug messenger");
        }
    }

    surface = Surface(instance, display.getWindow());

    auto pd = PhysicalDevice(PhysicalDevice(instance, surface.getVkSurfaceKHR()));
    auto ld = LogicalDevice(LogicalDevice(pd));
    device = Device(pd, ld);

    swapchain = Swapchain(display.getResX(), display.getResY(), device, surface.getVkSurfaceKHR());

    command = Command(device, concurrentFrames);

    renderPass = RenderPass
    (
      device, 
      device.getPhysicalDevice().getMaxMsaa(),
      swapchain.getImageFormat()
    );

    auto views = swapchain.getVkImageViews();
    
    createImage
    (
        device,
        swapchain.getVkExtend2D().width,
        swapchain.getVkExtend2D().height,
        1,
        device.getPhysicalDevice().getMaxMsaa(),
        swapchain.getImageFormat(),
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        framebufferImage,
        framebufferDeviceMemory
    );

    createImageView
    (
        device,
        swapchain.getImageFormat(),
        VK_IMAGE_ASPECT_COLOR_BIT,
        1,
        framebufferImage,
        framebufferImageView
    );

    framebuffers.reserve(views.size());

    for (auto & v : views)
    {
        framebuffers.push_back
        (
            std::move(std::make_unique<Framebuffer>
            (
                device,
                swapchain,
                renderPass,
                framebufferImageView,
                v
            ))
        );
    }

  }

  VulkanInstance::~VulkanInstance()
  {
    vkDestroyImageView(device.getVkDevice(), framebufferImageView, nullptr);
    vkDestroyImage(device.getVkDevice(), framebufferImage, nullptr);
    vkFreeMemory(device.getVkDevice(), framebufferDeviceMemory, nullptr);
    framebuffers.clear();
    
    framebuffers.clear();
    swapchain.cleanupSwapchain(device);

    vkDestroyRenderPass(device.getVkDevice(), renderPass.getVkRenderPass(), nullptr);

    // command buffers are also freed here
    vkDestroyCommandPool(device.getVkDevice(), command.getVkCommandPool(), nullptr);

    vkDestroyDevice(device.getVkDevice(), nullptr);

    vkDestroySurfaceKHR(instance, surface.getVkSurfaceKHR(), nullptr);

    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, nullptr);
    }

    vkDestroyInstance(instance, nullptr);
  }

  void VulkanInstance::supportedValidationLayers(bool print)
  {
      uint32_t layerCount;
      // get all the validation layers allowed
      vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

      availableLayers = std::vector<VkLayerProperties>(layerCount);
      vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

      if (print)
      {
          for (const auto & layerProperties : availableLayers)
          {
              std::cout << layerProperties.layerName << "\n";
          }
      }
  }

  bool VulkanInstance::checkValidationLayerSupport()
  {
      supportedValidationLayers(true);

      for (const char * layerName : validationLayers)
      {
          bool layerFound = false;

          for (const auto & layerProperties : availableLayers)
          {
              if (std::string(layerName) == std::string(layerProperties.layerName))
              {
                  layerFound = true;
                  break;
              }
          }

          if (!layerFound)
          {
              return false;
          }
      }

      return true;
  }

}