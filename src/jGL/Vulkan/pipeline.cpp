#include <jGL/Vulkan/pipeline.h>

namespace jGL::Vulkan
{
    Pipeline::Pipeline
    (
        const Device & device, 
        const RenderPass & renderPass,
        const vkShader & shader,
        std::vector<std::shared_ptr<VertexBufferObject>> vertexBuffers,
        std::vector<std::shared_ptr<UniformBufferObject>> uniforms,
        std::vector<std::pair<VkImageView, std::shared_ptr<Sampler> >> & textures,
        const uint32_t concurrentFrames,
        const VkPrimitiveTopology & topology,
        const VkViewport & viewport,
        const VkRect2D & scissor,
        const VkPolygonMode & polygonMode,
        const VkSampleCountFlagBits & msaaSamples,
        const VkPipelineColorBlendStateCreateInfo & blending
    )
    : device(device), 
      uniformDescriptorPool
      (
        device, 
        concurrentFrames, 
        std::vector<VkDescriptorType> (uniforms.size(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
      ),
      imageDescriptorPool
      (
        device,
        concurrentFrames,
        std::vector<VkDescriptorType> (textures.size(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
      )
    {

        createDescriptorSetLayouts(uniforms, textures, concurrentFrames);
        createDescriptorSets(uniforms, textures, concurrentFrames);

        auto shaderStages = shader.shaderStage();

        std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions;
        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
        
        for (std::shared_ptr<VertexBufferObject> input : vertexBuffers)
        {
            for (VkVertexInputBindingDescription binding : input->getBindingDescription())
            {
                vertexBindingDescriptions.push_back(binding);
            }

            for (VkVertexInputAttributeDescription attribute : input->getArrtibuteDescriptions())
            {
                vertexAttributeDescriptions.push_back(attribute);
            }
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = vertexBindingDescriptions.size();
        vertexInputInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
        vertexInputInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptions.size();
        vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = topology;
        // if true can break up line or triangle strips
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        std::vector<VkDynamicState> dynamicStates =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterInfo{};
        rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterInfo.depthClampEnable = VK_FALSE;
        // true disables framebuffer output
        rasterInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterInfo.polygonMode = polygonMode;
        // 1.0f requires wideLines enabled
        rasterInfo.lineWidth = 1.0f;
        rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterInfo.depthBiasEnable = VK_FALSE;
        rasterInfo.depthBiasConstantFactor = 0.0f;
        rasterInfo.depthBiasClamp = 0.0f;
        rasterInfo.depthBiasSlopeFactor = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_TRUE;
        multisampling.minSampleShading = 1.0f;
        multisampling.rasterizationSamples = msaaSamples;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts {uniformDescriptorSetLayout, imageDescriptorSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(device.getVkDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed ot create pipeline layout");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStages.size();
        // shaders
        pipelineInfo.pStages = shaderStages.data();
        // fixed functions
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterInfo;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &blending;
        pipelineInfo.pDynamicState = &dynamicState;
        
        pipelineInfo.layout = pipelineLayout;

        pipelineInfo.renderPass = renderPass.getVkRenderPass();
        pipelineInfo.subpass = 0;

        // inheritance
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(device.getVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipline");
        }
    }

    void Pipeline::createDescriptorSetLayouts
    (
        std::vector<std::shared_ptr<UniformBufferObject>> uniforms,
        std::vector<std::pair<VkImageView, std::shared_ptr<Sampler> >> & textures,
        const uint32_t concurrentFrames
    )
    {
        std::vector<VkDescriptorSetLayoutBinding> uniformBindingDescriptions;

        for (std::shared_ptr<UniformBufferObject> uni : uniforms)
        {
            uniformBindingDescriptions.push_back(uni->getLayout());
        }

        VkDescriptorSetLayoutCreateInfo uniformLayout{};
        uniformLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        uniformLayout.bindingCount = uniformBindingDescriptions.size();
        uniformLayout.pBindings = uniformBindingDescriptions.data();
        
        if (vkCreateDescriptorSetLayout(device.getVkDevice(), &uniformLayout, nullptr, &uniformDescriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not allocate descriptor set layout in pipeline");
        }

        std::vector<VkDescriptorSetLayoutBinding> imageBindingDescriptions;

        for (auto texSampler : textures)
        {
            imageBindingDescriptions.push_back(texSampler.second->getLayout());
        }

        VkDescriptorSetLayoutCreateInfo imageLayout{};
        imageLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        imageLayout.bindingCount = imageBindingDescriptions.size();
        imageLayout.pBindings = imageBindingDescriptions.data();
        
        if (vkCreateDescriptorSetLayout(device.getVkDevice(), &imageLayout, nullptr, &imageDescriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not allocate descriptor set layout in pipeline");
        }
    }

    void Pipeline::createDescriptorSets
    (
        std::vector<std::shared_ptr<UniformBufferObject>> uniforms,
        std::vector<std::pair<VkImageView, std::shared_ptr<Sampler> >> & textures,
        const uint32_t concurrentFrames
    )
    {

        std::vector<VkDescriptorSetLayout> uniformLayouts;
        // frames X UBO in layout
        std::vector<std::vector<VkDescriptorBufferInfo>> uniformInfo;
        std::vector<std::vector<VkDescriptorType>> uniformTypes;

        for (unsigned i = 0; i < concurrentFrames; i++)
        {
            uniformLayouts.push_back(uniformDescriptorSetLayout);
            std::vector<VkDescriptorBufferInfo> infos;
            std::vector<VkDescriptorType> types;

            for (auto UBO : uniforms)
            {
                infos.push_back(UBO->getBufferInfo());
                types.push_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            }
            uniformInfo.push_back(infos);
            uniformTypes.push_back(types);
        }

        uniformDescriptors = DescriptorSets<VkDescriptorBufferInfo>
        (
            device, 
            uniformDescriptorPool,
            uniformLayouts,
            uniformInfo,
            uniformTypes
        );

        std::vector<VkDescriptorSetLayout> imageLayouts;
        std::vector<std::vector<VkDescriptorImageInfo>> imageInfo;
        std::vector<std::vector<VkDescriptorType>> imageTypes;

        for (unsigned i = 0; i < concurrentFrames; i++)
        {
            imageLayouts.push_back(imageDescriptorSetLayout);
            std::vector<VkDescriptorImageInfo> infos;
            std::vector<VkDescriptorType> types;

            for (auto img : textures)
            {
                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = img.first;
                imageInfo.sampler = img.second->getVkSampler();
                infos.push_back(imageInfo);
                types.push_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            }
            imageInfo.push_back(infos);
            imageTypes.push_back(types);
        }

        imageDescriptors = DescriptorSets<VkDescriptorImageInfo>
        (
            device, 
            imageDescriptorPool,
            imageLayouts,
            imageInfo,
            imageTypes
        );
        
    }
}