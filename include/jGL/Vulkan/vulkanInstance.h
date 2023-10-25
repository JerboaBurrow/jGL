#ifndef VULKANINSTANCE
#define VULKANINSTANCE

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <version.h>
#include <jGL/Vulkan/vkDebug.h>

#include <jGL/Display/display.h>
#include <jGL/jGL.h>

#include <jGL/Vulkan/Device/device.h>
#include <jGL/Vulkan/surface.h>
#include <jGL/Vulkan/swapchain.h>
#include <jGL/Vulkan/Command/command.h>
#include <jGL/Vulkan/RenderPass/renderPass.h>
#include <jGL/Vulkan/framebuffer.h>
#include <jGL/Vulkan/pipeline.h>

#include <jGL/Vulkan/vkParticles.h>
#include <jGL/Vulkan/Texture/vkTexture.h>

namespace jGL::Vulkan
{
    class vkTexture;

    class VulkanInstance : public jGLInstance
    {

    public:

        VulkanInstance(const Display & display);

        ~VulkanInstance();

        void text
        (
            std::string characters, 
            glm::vec2 position,
            float scale,
            glm::vec4 colour
        ){/*TODO*/}

        void clear() {/*TODO*/}

        void setClear(glm::vec4 colour) {/*TODO*/}
        void setProjection(glm::mat4 proj) {/*TODO*/}
        void setTextProjection(glm::mat4) {/*TODO*/}

        std::shared_ptr<Particles> createParticles(size_t sizeHint) 
        { 
            return std::static_pointer_cast<Particles>(std::make_shared<vkParticles>(sizeHint)); 
        }
        std::shared_ptr<Texture> createTexture(std::string imageFile, Texture::Type type) 
        { 
            return std::static_pointer_cast<Texture>
            (
                std::make_shared<vkTexture>
                (
                    device, 
                    command,
                    imageFile, 
                    type
                )
            );   
        }

        const Device & getDevice() const { return device; }
        const Command & getCommand() const { return command; }
        const VkInstance & getVkInstance() const { return instance; }
        const Swapchain & getSwapchain() const { return swapchain; }
        const RenderPass & getRenderPass() const { return renderPass; }
        const unsigned getConcurrentFrames() const { return concurrentFrames; }

    private:

        const unsigned concurrentFrames = 2;

        VkInstance instance;

        VkDebugUtilsMessengerEXT debugMessenger;

        std::vector<VkLayerProperties> availableLayers;

        Surface surface;
        Device device;
        Swapchain swapchain;
        Command command;
        RenderPass renderPass;
        std::vector<std::unique_ptr<Framebuffer>> framebuffers;

        VkImage framebufferImage;
        VkImageView framebufferImageView;
        VkDeviceMemory framebufferDeviceMemory;

        void supportedValidationLayers(bool print = false);
        bool checkValidationLayerSupport();
        
    };
}

#endif /* VULKANINSTANCE */
