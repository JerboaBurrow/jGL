#include <jGL/Vulkan/Shader/vkShader.h>

namespace jGL::Vulkan
{

    vkShader::~vkShader()
    {
        vkDestroyShaderModule(device, fragmentModule, nullptr);
        vkDestroyShaderModule(device, vertexModule, nullptr);
    }

    void vkShader::compile()
    {
        shaderc::CompileOptions options;

        auto preprocessedVert = preprocessShader
        (
            "shader_src", 
            shaderc_glsl_vertex_shader, 
            vertex.c_str(), 
            options
        );
    
        vertexSource = compileSPIRV
        (
            "shader_src", 
            shaderc_glsl_vertex_shader, 
            preprocessedVert, 
            options,
            true
        );

        auto preprocessedFrag = preprocessShader
        (
            "shader_src", 
            shaderc_glsl_fragment_shader, 
            fragment.c_str(), 
            options
        );

        fragmentSource = compileSPIRV
        (
            "shader_src", 
            shaderc_glsl_fragment_shader, 
            preprocessedFrag, 
            options,
            true
        );
    }

    std::vector<char> vkShader::readSPIRV(const std::string & filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open SPIR-V file at "+filename);
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    void vkShader::createShaderModules(const VkDevice & device)
    {
        VkShaderModuleCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = vertexSource.size()*4;
        createInfo.pCode = reinterpret_cast<const uint32_t*>(vertexSource.data());

        if (vkCreateShaderModule(device, &createInfo, nullptr, &vertexModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vertex vkShader module");
        }

        createInfo.codeSize = fragmentSource.size()*4;
        createInfo.pCode = reinterpret_cast<const uint32_t*>(fragmentSource.data());

        if (vkCreateShaderModule(device, &createInfo, nullptr, &fragmentModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create fragment vkShader module");
        }
    }

    std::vector<VkPipelineShaderStageCreateInfo> vkShader::shaderStage() const
    {
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertexModule;
        // the entry point
        vertShaderStageInfo.pName = "main";
        // can set constant on-the-fly
        // vertShaderStageInfo.pSpecializationInfo

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragmentModule;
        // the entry point
        fragShaderStageInfo.pName = "main";

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = 
        {
            vertShaderStageInfo,
            fragShaderStageInfo
        };

        return shaderStages;
    }

    std::string vkShader::preprocessShader
    (
        const std::string& source_name,
        shaderc_shader_kind kind,
        const std::string& source,
        shaderc::CompileOptions options
    ) 
    {
        shaderc::Compiler compiler;

        shaderc::PreprocessedSourceCompilationResult result = compiler.PreprocessGlsl
        (
            source, 
            kind, 
            source_name.c_str(), 
            options
        );

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) 
        {
            std::cerr << result.GetErrorMessage();
            return "";
        }

        return {result.cbegin(), result.cend()};
    }

    std::string vkShader::compileToAssembly
    (
        const std::string& source_name,
        shaderc_shader_kind kind,
        const std::string& source,
        shaderc::CompileOptions options,
        bool optimize
    ) 
    {
        shaderc::Compiler compiler;

        if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

        shaderc::AssemblyCompilationResult result = compiler.CompileGlslToSpvAssembly
        (
            source, 
            kind, 
            source_name.c_str(), 
            options
        );

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) 
        {
            std::cerr << result.GetErrorMessage();
            return "";
        }

        return {result.cbegin(), result.cend()};
    }

    std::vector<uint32_t> vkShader::compileSPIRV
    (
        const std::string& source_name,
        shaderc_shader_kind kind,
        const std::string& source,
        shaderc::CompileOptions options,
        bool optimize
    ) 
    {

        shaderc::Compiler compiler;
    
        if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv
        (
            source, 
            kind, 
            source_name.c_str(), 
            options
        );

        if (module.GetCompilationStatus() != shaderc_compilation_status_success) 
        {
            std::cerr << module.GetErrorMessage();
            return std::vector<uint32_t>();
        }

        return {module.cbegin(), module.cend()};

    }
}