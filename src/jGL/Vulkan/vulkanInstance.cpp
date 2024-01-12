#include <jGL/Vulkan/vulkanInstance.h>

namespace jGL::Vulkan
{

    VulkanInstance::VulkanInstance(const DesktopDisplay & display)
    : jGLInstance(display.getRes())
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

        VkExtent2D swapchainExtent = swapchain.getVkExtend2D();

        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapchainExtent.width;
        viewport.height = (float) swapchainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        scissor.offset = {0, 0};
        scissor.extent = swapchainExtent;

        textRenderer = std::make_unique<TextRenderer>
        (
            device,
            command,
            renderPass,
            viewport,
            scissor,
            concurrentFrames,
            device.getPhysicalDevice().getMaxMsaa()
        );

        createSyncObjects();

    }

    VulkanInstance::~VulkanInstance()
    {
        textRenderer.reset();
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

    void VulkanInstance::beginFrame()
    {
        vkWaitForFences
        (
            device.getVkDevice(), 
            1, 
            &framesFinished[currentFrame], 
            VK_TRUE, 
            UINT64_MAX
        );

        if (closing){return;}

        // aquire an image
        VkResult result = vkAcquireNextImageKHR
        (
            device.getVkDevice(), 
            swapchain.getVkSwapchainKHR(), 
            UINT64_MAX, 
            imageAvailableSemaphores[currentFrame], 
            VK_NULL_HANDLE, 
            &swapchainImageIndex
        );

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            swapchain.recreateSwapchain(resolution.x, resolution.y, device, surface.getVkSurfaceKHR());
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to aquire swap chain image");
        }

        vkResetFences(device.getVkDevice(), 1, &framesFinished[currentFrame]);
        commandBuffer = command.getVkCommandBuffer(currentFrame);
        vkResetCommandBuffer(commandBuffer, 0);
        
        VkCommandBufferBeginInfo beginInfo{};

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;
        
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin recording command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass.getVkRenderPass();
        renderPassInfo.framebuffer = framebuffers[swapchainImageIndex]->getVkFramebuffer();
        // size of the render area for shader 
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain.getVkExtend2D();
        // set the clear values fo VK_ATTACHMENT_LOAD_OP_CLEAR;
        VkClearValue clear = {{{clearColour.r, clearColour.g, clearColour.b, clearColour.a}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clear;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            // default to instance's default viewport and scissor
            //  can be changed before in any commands before endFrame
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    
    }
    
    void VulkanInstance::endFrame()
    {
        if (closing){return;}
        vkCmdEndRenderPass(commandBuffer);
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer");
        }

        // submit the command buffer 
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        // wait on this semaphore
        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        // in this stage of the pipline
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        VkSemaphore signalSemaphores[] = {renderFinsihedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(device.getLogicalDevice().getGraphicsQueue(), 1, &submitInfo, framesFinished[currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit draw comamnd buffer");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapchains[] = {swapchain.getVkSwapchainKHR()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &swapchainImageIndex;
        // can check on success
        presentInfo.pResults = nullptr;

        VkResult result = vkQueuePresentKHR(device.getLogicalDevice().getPresentQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            swapchain.recreateSwapchain(resolution.x, resolution.y, device, surface.getVkSurfaceKHR());
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present swap chain image");
        }

        currentFrame = (currentFrame+1)%concurrentFrames;
    }

    void VulkanInstance::createSyncObjects()
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType =  VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        // begin signalled
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        imageAvailableSemaphores.resize(concurrentFrames);
        renderFinsihedSemaphores.resize(concurrentFrames);
        framesFinished.resize(concurrentFrames);

        for (unsigned i = 0; i < concurrentFrames; i++)
        {
            if 
            (
                vkCreateSemaphore(device.getVkDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device.getVkDevice(), &semaphoreInfo, nullptr, &renderFinsihedSemaphores[i]) != VK_SUCCESS
            )
            {
                throw std::runtime_error("Failed to create semaphore");
            }

            if (vkCreateFence(device.getVkDevice(), &fenceInfo, nullptr, &framesFinished[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create fence");
            }
        }


    }
}