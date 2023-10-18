#include <jGL/Vulkan/Text/textRenderer.h>

namespace jGL::Vulkan
{
    TextRenderer::TextRenderer
    (
        const Device & device,
        const Command & command,
        const RenderPass & renderPass,
        const VkViewport & viewport,
        const VkRect2D & scissor,
        uint32_t concurrentFrames,
        VkSampleCountFlagBits msaa
    )
    : shader(device.getVkDevice(), vert, frag)
    {

        font = std::make_unique<Font>
        (
            device,
            command,
            48
        );

        vertices = font->getGlyphVertices(0.0f, 0.0f, 1.0f, 'H');

        posTex = std::make_shared<VertexBuffer<glm::vec4>>
        (
            device, 
            command,
            std::vector<glm::vec4> {vertices.begin(), vertices.end()},
            VK_VERTEX_INPUT_RATE_VERTEX,
            0,
            0
        );

        auto vertexInputs = {std::static_pointer_cast<VertexBufferObject>(posTex)};

        uboV = std::make_shared<UniformBuffer<vUBO>>
        (
            device,
            vUBO {glm::mat4(0.0)},
            0,
            VK_SHADER_STAGE_VERTEX_BIT
        );

        uboF = std::make_shared<UniformBuffer<fUBO>>
        (
            device,
            fUBO {glm::vec4(0.0)},
            1,
            VK_SHADER_STAGE_FRAGMENT_BIT
        );

        auto uniforms = {
            std::static_pointer_cast<UniformBufferObject>(uboV),
            std::static_pointer_cast<UniformBufferObject>(uboF)
        };

        fontTexture = std::make_shared<vkTexture>
        (
            device,
            command,
            64,
            64,
            1,
            VK_FORMAT_R8_UINT
        );

        fontSampler = std::make_shared<Sampler>
        (
            device,
            VK_FILTER_NEAREST,
            0
        );

        std::vector<std::pair<VkImageView, std::shared_ptr<Sampler> >> textures
            {std::pair(font->getGlyphView('H'), fontSampler)};


        VkPipelineColorBlendAttachmentState colourBlendAttachment{};
        colourBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colourBlendAttachment.blendEnable = VK_TRUE;
        colourBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colourBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colourBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colourBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colourBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colourBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;


        VkPipelineColorBlendStateCreateInfo blending {};
        blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        blending.logicOpEnable = VK_FALSE;
        blending.logicOp = VK_LOGIC_OP_COPY; 
        blending.attachmentCount = 1;
        blending.pAttachments = &colourBlendAttachment;
        blending.blendConstants[0] = 0.0f; 
        blending.blendConstants[1] = 0.0f; 
        blending.blendConstants[2] = 0.0f;
        blending.blendConstants[3] = 0.0f;


        textPipeline = std::make_unique<Pipeline>
        (
            device,
            renderPass,
            shader,
            vertexInputs,
            uniforms,
            textures,
            concurrentFrames,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            viewport,
            scissor,
            VK_POLYGON_MODE_FILL,
            msaa,
            blending
        );
    }

    void TextRenderer::renderText
    (
        const VkCommandBuffer & commandBuffer,
        uint32_t currentFrame,
        std::string text,
        glm::vec2 position,
        float scale,
        glm::vec4 colour,
        bool centre
    )
    {

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, textPipeline->getVkPipeline());

        VkBuffer vertexBuffers[] = {posTex->getVkBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        std::vector<VkDescriptorSet> descriptorSets = textPipeline->getVkDescriptorSets(currentFrame);
        
        vkCmdBindDescriptorSets
        (
            commandBuffer, 
            VK_PIPELINE_BIND_POINT_GRAPHICS, 
            textPipeline->getVkPipelineLayout(),
            0,
            descriptorSets.size(),
            &descriptorSets[0],
            0,
            nullptr
        );

        // the draw command is issues
        // vertexCount, instanceCount, firstVertex, firstInstance
        vkCmdDraw(commandBuffer, vertices.size(), 1, 0, 0);
    }
}