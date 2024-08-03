#include <jGL/OpenGL/glShapeRenderer.h>

namespace jGL::GL 
{

    const char * glShapeRenderer::shapeVertexShader = 
        "#version " GLSL_VERSION "\n"
        "precision lowp float; precision lowp int;\n"
        "layout(location=0) in vec4 a_position;\n"
        "layout(location=1) in vec3 a_xytheta;\n"
        "layout(location=2) in vec2 a_scale;\n"
        "layout(location=3) in vec4 a_colour;\n"
        "uniform mat4 proj;\n"
        "out vec2 texCoord;\n"
        "out vec4 colour;\n"
        "void main(){"
            "vec2 pos = a_position.xy*a_scale;\n"
            "float ct = cos(a_xytheta.z); float st = sin(a_xytheta.z);\n"
            "mat2 rot = mat2(ct, -st, st, ct);\n"
            "pos = rot*pos + a_xytheta.xy;\n"
            "gl_Position = proj*vec4(pos,0.0,1.0);\n"
            "texCoord = a_position.zw;\n"
            "colour = a_colour;\n"
        "}";

    const char * glShapeRenderer::rectangleFragmentShader = 
        "#version " GLSL_VERSION "\n"
        "precision lowp float; precision lowp int;\n"
        "in vec2 texCoord;\n"
        "in vec4 colour;\n"
        "layout(location=0) out vec4 fragment;\n"
        "void main(){\n" 
            "fragment = colour;\n"
        "}";

    const char * glShapeRenderer::ellipseFragmentShader = 
        "#version " GLSL_VERSION "\n"
        "precision lowp float; precision lowp int;\n"
        "in vec2 texCoord;\n"
        "in vec4 colour;\n"
        "out vec4 fragment;\n"
        "void main(void){\n" 
            "vec2 c = texCoord-vec2(0.5,0.5);\n"
            "if (dot(c,c) > 0.5*0.5) {discard;}\n"
            "fragment = colour;\n"
        "}";

    void glShapeRenderer::draw(std::shared_ptr<Shader> shader, std::multimap<RenderPriority, ShapeId> ids)
    {

        uint32_t n = ids.size();

        if (xytheta.size() < 4*n)
        {
            xytheta.resize(xythetaDim*n+padShapes*xythetaDim);
            scale.resize(2*n+padShapes*2);
            colours.resize(coloursDim*n+padShapes*coloursDim);
            freeGL();
            initGL();
        }

        uint64_t i = 0;
        for (auto & sid : ids)
        {
            const auto shape = shapes[sid.second];
            const Transform & trans = shape->transform;
            const glm::vec4 & col = shape->colour;

            xytheta[i*xythetaDim] = trans.x;
            xytheta[i*xythetaDim+1] = trans.y;
            xytheta[i*xythetaDim+2] = trans.theta;

            scale[i*scaleDim] = trans.scaleX;
            scale[i*scaleDim+1] = trans.scaleY;

            colours[i*coloursDim] = col.r;
            colours[i*coloursDim+1] = col.g;
            colours[i*coloursDim+2] = col.b;
            colours[i*coloursDim+3] = col.a;

            i += 1;
        }

        shader->use();
        shader->setUniform<glm::mat4>("proj", projection);

        glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, a_xytheta);
                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    xythetaDim*n*sizeof(float),
                    &xytheta[0]
                );
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_scale);
                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    scaleDim*n*sizeof(float),
                    &scale[0]
                );
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            glBindBuffer(GL_ARRAY_BUFFER, a_colour);

                glBufferSubData
                (
                    GL_ARRAY_BUFFER,
                    0,
                    coloursDim*n*sizeof(float),
                    &colours[0]
                );

            glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        glBindVertexArray(vao);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);
        
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, n);

        glBindVertexArray(0);

    }

    void glShapeRenderer::freeGL()
    {
        glDeleteBuffers(1, &a_position);
        glDeleteBuffers(1, &a_xytheta);
        glDeleteBuffers(1, &a_scale);
        glDeleteBuffers(1, &a_colour);
        glDeleteVertexArrays(1, &vao);
    }

    void glShapeRenderer::initGL()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &a_position);
        glGenBuffers(1, &a_xytheta);
        glGenBuffers(1, &a_scale);
        glGenBuffers(1, &a_colour);

        glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, a_position);
                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    sizeof(float)*6*4,
                    &quad[0],
                    GL_STATIC_DRAW
                );
                glEnableVertexAttribArray(0);
                glVertexAttribPointer
                (
                    0,
                    4,
                    GL_FLOAT,
                    false,
                    4*sizeof(float),
                    0
                );
                glVertexAttribDivisor(0,0);
    
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_xytheta);

                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    sizeof(float)*xytheta.size(),
                    xytheta.data(),
                    GL_DYNAMIC_DRAW
                );

                glEnableVertexAttribArray(xythetaAttribtue);
                glVertexAttribPointer
                (
                    xythetaAttribtue,
                    xythetaDim,
                    GL_FLOAT,
                    false,
                    xythetaDim*sizeof(float),
                    0
                );
                glVertexAttribDivisor(xythetaAttribtue, 1);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_scale);

                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    sizeof(float)*scale.size(),
                    scale.data(),
                    GL_DYNAMIC_DRAW
                );

                glEnableVertexAttribArray(scaleAttribtue);
                glVertexAttribPointer
                (
                    scaleAttribtue,
                    scaleDim,
                    GL_FLOAT,
                    false,
                    scaleDim*sizeof(float),
                    0
                );
                glVertexAttribDivisor(scaleAttribtue, 1);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_colour);

                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    sizeof(float)*colours.size(),
                    colours.data(),
                    GL_DYNAMIC_DRAW
                );

                glEnableVertexAttribArray(coloursAttribtue);
                glVertexAttribPointer
                (
                    coloursAttribtue,
                    coloursDim,
                    GL_FLOAT,
                    false,
                    coloursDim*sizeof(float),
                    0
                );
                glVertexAttribDivisor(coloursAttribtue, 1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);


        glBindVertexArray(0);
    }
}