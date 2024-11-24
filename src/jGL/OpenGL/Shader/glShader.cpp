#include <jGL/OpenGL/Shader/glShader.h>

namespace jGL::GL
{

    void glShader::create()
    {
        if (!isProgram())
        {
            program = glCreateProgram();
            compiled = false;
        }
    }

    void glShader::release()
    {
        if (isProgram())
        {
            glDeleteProgram(program);
            compiled = false;
        }
    }

    void glShader::compile()
    {
        if (!isProgram())
        {
            create();
        }

        parseUniforms();

        GLuint vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        auto v = vertex.c_str();
        glShaderSource(vertexShader,1,&v,NULL);
        glCompileShader(vertexShader);

        // check it worked!
        int  success;
        const unsigned logSize = 512*4;
        char infoLog[logSize];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

        if(!success)
        {
            glGetShaderInfoLog(vertexShader, logSize, NULL, infoLog);
            throw
            (
                GLRuntimeException
                (
                    std::string("GLSL (VERTEX) ERROR: \n") + infoLog + "\n"+displayVertexSource()+"\n"
                )
            );
        }

        GLuint fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        auto f = fragment.c_str();
        glShaderSource(fragmentShader,1,&f,NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

        if(!success)
        {
            glGetShaderInfoLog(fragmentShader, logSize, NULL, infoLog);
            throw
            (
                GLRuntimeException
                (
                    std::string("GLSL (FRAGMENT) ERROR: \n") + infoLog + "\n"+displayFragmentSource()+"\n"
                )
            );
        }

        glAttachShader(program,vertexShader);
        glAttachShader(program,fragmentShader);
        glLinkProgram(program);

        // check it linked
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(program, logSize, NULL, infoLog);
            throw
            (
                GLRuntimeException
                (
                    std::string("GLSL (LINK) ERROR: \n") + infoLog + "\n"+displayVertexSource()+"\n"+displayFragmentSource()+"\n"
                )
            );
        }
        glGetProgramInfoLog(program, logSize, NULL, infoLog);

        compiled = true;

        for (auto uniform = uniforms.cbegin(); uniform != uniforms.cend(); uniform++)
        {

            AbstractjGLUniform * u = (*uniform).second.get();

            jGLUniform<int> * ui = dynamic_cast<jGLUniform<int>*>(u);
            if (ui != nullptr){ setValue(ui, 0); continue; }

            jGLUniform<float> * uf = dynamic_cast<jGLUniform<float>*>(u);
            if (uf != nullptr){ setValue(uf, 0.0f); continue; }

            jGLUniform<glm::vec2> * uv = dynamic_cast<jGLUniform<glm::vec2>*>(u);
            if (uv != nullptr){ setValue(uv, glm::vec2(0.0f)); continue; }

            jGLUniform<glm::mat4> * um = dynamic_cast<jGLUniform<glm::mat4>*>(u);
            if (um != nullptr){ setValue(um, glm::mat4(0.0f)); continue; }
            
        }

    }

    void glShader::use()
    {
        if (!compiled){compile();}
        glUseProgram(program);
    }
}