#include <stdint.h>
#include <iostream>
#include <random>
const double tol = 1e-6;
#include <cmath>

#include <jGL/Display/desktopDisplay.h>
#include <jGL/Vulkan/vulkanInstance.h>
#include <jGL/texture.h>
#include <jGL/Vulkan/Shader/vkShader.h>
#include <jGL/Vulkan/Text/textRenderer.h>
#include <jGL/Vulkan/Texture/vkTexture.h>

const char *  TESTFS01 = "#version 450\n"
    "layout(location = 0) in vec3 fragColour;\n"
    "layout(location = 0) out vec4 outColour;\n"
    "void main()\n"
    "{\n"
    "    outColour = vec4(fragColour, 1.0);\n"
    "}\n";

const char * TESTVS01 = "#version 450\n"
    "layout(binding = 0) uniform UniformBufferObject\n"
    "{\n"
    "    mat4 model;\n"
    "    mat4 view;\n"
    "    mat4 proj;\n"
    "} ubo;\n"
    "layout(location = 0) in vec2 a_position;\n"
    "layout(location = 1) in vec3 a_colour;\n"
    "layout(location = 0) out vec3 fragColour;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(a_position, 0.0, 1.0);\n"
    "    fragColour = a_colour;\n"
    "}";

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

SCENARIO("Vulkan instance", "[vulkan]")
{
    WHEN("A VulkanInstance is created")
    {
        jGL::DesktopDisplay display(glm::ivec2(1, 1), "Vulkan Test", true);
        std::unique_ptr<jGL::Vulkan::VulkanInstance> vkInstance;
        bool instantCreated = false;
        try 
        {
            vkInstance = std::move(std::make_unique<jGL::Vulkan::VulkanInstance>(display));
            instantCreated = true;
        }
        catch (const std::exception & e)
        {
            std::cout << e.what() << "\n";
        }
        THEN("A Vulkan instance is created")
        {
            REQUIRE(instantCreated);
        }

        WHEN("Texture resource/texture/saturn.png is loaded")
        {
            bool textureCreated = false;
            try
            {
                std::shared_ptr<jGL::Texture> saturn = vkInstance->createTexture
                (
                    "resource/texture/saturn.png",
                    jGL::Texture::Type::RGBA
                );
                textureCreated = true;
            }
            catch (const std::exception & e)
            {
                std::cout << e.what() << "\n";
            }
            THEN("The texture is created")
            {
                REQUIRE(textureCreated);
            }
        }

        WHEN("An empty texture is instanced")
        {
            bool textureCreated = false;
            try
            {
                auto tex = std::make_shared<jGL::Vulkan::vkTexture>
                (
                    vkInstance->getDevice(),
                    vkInstance->getCommand(),
                    64,
                    64,
                    1,
                    VK_FORMAT_R8_SINT
                );
                textureCreated = true;
            }
            catch (const std::exception & e)
            {
                std::cout << e.what() << "\n";
            }
            THEN("The texture is created")
            {
                REQUIRE(textureCreated);
            }
        }

        WHEN("The default font is loaded")
        {
            bool fontLoaded = false;
            try
            {
                jGL::Vulkan::vkFont(vkInstance->getDevice(), vkInstance->getCommand(), 48);
                fontLoaded = true;
            }
            catch (const std::exception & e)
            {
                std::cout << e.what() << "\n";
            }
            THEN("The font is successfully loaded")
            {
                REQUIRE(fontLoaded);
            }
        }

        WHEN("A Shader is instanced with TESTVS01 and TESTFS01")
        {
            bool shaderCompiled = false;
            try
            {
                jGL::Vulkan::vkShader(vkInstance->getDevice().getVkDevice(), TESTVS01, TESTFS01);
                shaderCompiled = true;
            }
            catch (const std::exception & e)
            {
                std::cout << e.what() << "\n";
            }
            THEN("The shader is successfully compiled")
            {
                REQUIRE(shaderCompiled);
            }
        }

        WHEN("A TextRenderer is instanced")
        {
            bool loaded = false;
            try 
            {
                auto swapChainExtent = vkInstance->getSwapchain().getVkExtend2D();

                VkViewport viewport {};
                VkRect2D scissor {};

                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = (float) swapChainExtent.width;
                viewport.height = (float) swapChainExtent.height;
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;

                scissor.offset = {0, 0};
                scissor.extent = swapChainExtent;

                jGL::Vulkan::TextRenderer
                (
                    vkInstance->getDevice(),
                    vkInstance->getCommand(),
                    vkInstance->getRenderPass(),
                    viewport,
                    scissor,
                    vkInstance->getConcurrentFrames(),
                    vkInstance->getDevice().getPhysicalDevice().getMaxMsaa()
                );
                loaded = true;
            }
            catch (const std::exception & e)
            {
                std::cout << e.what() << "\n";
            }
            THEN("The textRenderer is successfully instanced")
            {
                REQUIRE(loaded);
            }
        }

        glfwSetWindowShouldClose(display.getWindow(), GLFW_TRUE);
        display.loop();
        vkInstance->finish();
        display.close();
    }
}

