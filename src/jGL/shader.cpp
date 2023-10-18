#include <jGL/shader.h>

namespace jGL
{
    Shader::Shader(std::string path, std::string name)
    {
        std::ifstream fileVs(path+name+".vs");
        std::ifstream fileFs(path+name+".fs");
        if (fileVs.is_open() && fileFs.is_open())
        {
            vertex = parseShaderSource(fileVs);
            fragment = parseShaderSource(fileFs);
        }
        else
        {
            throw std::runtime_error(" attempting to locate source files .vs and .fs at "+path+name);
        }

        parseUniforms();
    }

    std::string Shader::parseShaderSource(std::ifstream & file)
    {
        std::string src = "";
        std::string line;
        while (std::getline(file,line))
        {
            src += line + "\n";
        }
        return src;
    }

    bool Shader::parseUniforms()
    {
        std::regex UNIFORM_REGEX("uniform");
        bool fragmentNoUniforms = false;
        auto start = std::sregex_iterator(fragment.begin(), fragment.end(), UNIFORM_REGEX);
        auto end = std::sregex_iterator();
        if (std::distance(start, end) == 0)
        {
            fragmentNoUniforms = true;
        }

        start = std::sregex_iterator(vertex.begin(), vertex.end(), UNIFORM_REGEX);
        end = std::sregex_iterator();
        if (std::distance(start, end) == 0)
        {
            if (fragmentNoUniforms)
            {
                return true;
            }
        }

        std::vector<std::string> toCheck {vertex, fragment}; 

        for (unsigned i = 0; i < toCheck.size(); i++)
        {
            std::string code = toCheck[i];
            
            detectUniformsAndCreate<int>(code);
            detectUniformsAndCreate<float>(code);
            detectUniformsAndCreate<glm::vec2>(code);
            detectUniformsAndCreate<glm::mat4>(code);
            detectUniformsAndCreate<Sampler2D>(code);
        }

        return true;

    }

    bool operator==(const Shader & lhs, const Shader & rhs)
    {
        return lhs.getVertex() == rhs.getVertex() && lhs.getFragment() == rhs.getFragment();
    }

    bool Shader::lint()
    {
        Log log;
        if (vertex == "" || fragment == "")
        {
            WARN("Attempted to lint an empty shader") >> log;
            return true;
        }

        std::string msg("");

        std::string vs(vertex);
        std::string fs(fragment);

        std::string vsPreamble = vs.substr(0,vs.find("main"));
        std::string fsPreamble = fs.substr(0,fs.find("main"));

        if (vsPreamble.find("layout(location=") == std::string::npos)
        {
            msg += "\nCould not find layout specifiers in vertex shader, android requires them";
        }

        if (vsPreamble.find("precision") == std::string::npos)
        {
            msg += "\nCould not find precision qualifiers in vertex shader, optional";
        }

        if (fsPreamble.find("precision") == std::string::npos)
        {
            msg += "\nCould not find precision qualifiers in fragment shader, optional";
        }

        if (msg != "")
        {
            WARN(msg) >> log;
            return true;
        }
        else
        {
            return false;
        }
    }
}