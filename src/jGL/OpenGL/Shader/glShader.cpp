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

        compileShader(program,vertex.c_str(),fragment.c_str());
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