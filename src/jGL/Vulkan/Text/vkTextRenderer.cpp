#include <jGL/Vulkan/Text/vkTextRenderer.h>

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
    : 
        shader(device.getVkDevice(), vert, frag), 
        vertices(std::vector<glm::vec4>()),
        res(glm::ivec2(viewport.width, viewport.height))
    {

        font = std::make_unique<vkFont>
        (
            device,
            command,
            48
        );

        vertices = std::vector<glm::vec4>(256, glm::vec4(0.0));

        pos = std::make_shared<VertexBuffer<glm::vec4>>
        (
            device, 
            command,
            std::vector<glm::vec4> {vertices.begin(), vertices.end()},
            VK_VERTEX_INPUT_RATE_VERTEX,
            0,
            0
        );

        auto vertexInputs = 
        {
            std::static_pointer_cast<VertexBufferObject>(pos)
        };

        uboV = std::make_shared<UniformBuffer<vUBO>>
        (
            device,
            vUBO {glm::ortho(0.0,double(viewport.width),0.0,double(viewport.height))},
            0,
            VK_SHADER_STAGE_VERTEX_BIT
        );

        uboF = std::make_shared<UniformBuffer<fUBO>>
        (
            device,
            fUBO {glm::vec4(0.0, 0.0, 0.0, 1.0)},
            1,
            VK_SHADER_STAGE_FRAGMENT_BIT
        );

        auto uniforms = {
            std::static_pointer_cast<UniformBufferObject>(uboV),
            std::static_pointer_cast<UniformBufferObject>(uboF)
        };

        fontSampler = std::make_shared<Sampler>
        (
            device,
            VK_FILTER_NEAREST,
            0
        );

        std::vector<std::pair<VkImageView, std::shared_ptr<Sampler> >> textures
            {std::pair(font->getGlyphView(), fontSampler)};


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

    void TextRenderer::setProjection(glm::mat4 p)
    {
        auto ubo = std::static_pointer_cast<UniformBuffer<vUBO>>(uboV);
        vUBO data {p};
        ubo->set(data);
    }

    void TextRenderer::renderText
    (
        const Device & device,
        const Command & command,
        const VkCommandBuffer & commandBuffer,
        uint32_t currentFrame,
        std::string text,
        glm::vec2 position,
        float scale,
        glm::vec4 colour,
        glm::bvec2 centre
    )
    {

        auto ubo = std::static_pointer_cast<UniformBuffer<fUBO>>(uboF);
        fUBO data {colour};
        ubo->set(data);

        if (vertices.size() < text.size()*6)
        {
            vertices = std::vector<glm::vec4>(text.size()*6, glm::vec4(0.0));
        }

        glm::vec2 maxWidth = glm::vec2(0.0f);
        glm::vec2 width = glm::vec2(0.0f);
        float p = position.x;
        float y = res.y-position.y;
        unsigned i = 0;
        for (unsigned char ch : text)
        {
            if (ch == 0x20)
            {
                p += font->spacing(scale);
                width.x += font->spacing(scale);
            }
            else if (ch == 0x0A)
            {
                y += font->spacing(scale);
                p = position.x;

                maxWidth.y += font->spacing(scale);
                maxWidth.x = std::max(width.x, maxWidth.x);
                width.x = 0;
            }
            else
            {
                for(auto vertex : font->getGlyphVertices(p, y, scale, ch))
                {
                    vertices[i] = vertex;
                    i++;
                }
                p += scale*(font->getGlyphSize(ch).x+1);
                width.x += scale*(font->getGlyphSize(ch).x+1);
            } 
        }

        maxWidth.x = std::max(width.x, maxWidth.x);

        if (centre.x || centre.y)
        {
          unsigned j = 0;
          
          glm::vec2 correction = glm::vec2
          (
            centre.x ? maxWidth.x * 0.5f : 0.0f, 
            centre.y ? maxWidth.y * 0.5f : 0.0f
          );

          while (j < i)
          {
              vertices[j] -= correction.x;
              vertices[j+1] += correction.y;
              j+=4;
          }
        }

        // this is bad, lol...
        vkQueueWaitIdle(device.getLogicalDevice().getGraphicsQueue());
        std::static_pointer_cast<VertexBuffer<glm::vec4>>(pos)->subData(device, command, vertices);
        vkQueueWaitIdle(device.getLogicalDevice().getGraphicsQueue());

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, textPipeline->getVkPipeline());

        VkBuffer vertexBuffers[] = {pos->getVkBuffer()};
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
        vkCmdDraw(commandBuffer, i, 1, 0, 0);
    }
}