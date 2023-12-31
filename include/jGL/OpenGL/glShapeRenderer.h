#ifndef GLSHAPERENDERER_H
#define GLSHAPERENDERER_H

#include <jGL/OpenGL/Shader/glShader.h>
#include <jGL/OpenGL/gl.h>
#include <jGL/shapeRenderer.h>

namespace jGL::GL
{
    class glShapeRenderer : public ShapeRenderer
    {

    public:
    
        glShapeRenderer(size_t sizeHint = 8)
        : ShapeRenderer(sizeHint)
        {
            offsets = std::vector<float>(sizeHint*4+padShapes*4,0.0f);
            colours = std::vector<float>(sizeHint*4+padShapes*4,0.0f);
            initGL();
            defaultShader = std::make_shared<glShader>(vertexShader, fragmentShader);
            defaultShader->use();
        }

        ~glShapeRenderer()
        {
            freeGL();
        }

        void draw(std::shared_ptr<Shader> shader, std::vector<ShapeId> ids);
        void draw(std::vector<ShapeId> ids) { draw(defaultShader, ids); }

    private:

        GLuint vao, a_position, a_offset, a_colour;

        float quad[6*4] = 
        {
            // positions  / texture coords
            0.5f,  0.5f, 1.0f, 1.0f,   // top right
            0.5f,  -0.5f, 1.0f, 0.0f,   // bottom right
            -0.5f,  -0.5f, 0.0f, 0.0f,   // bottom left
            -0.5f,  0.5f, 0.0f, 1.0f,    // top left 
            -0.5f,  -0.5f, 0.0f, 0.0f,   // bottom left
            0.5f,  0.5f, 1.0f, 1.0f  // top right
        };

        std::vector<float> offsets;         // offset x, y, theta, scale
        std::vector<float> colours;         // r, g, b, a

        size_t padShapes = 8;

        void initGL();
        void freeGL();

        std::shared_ptr<Shader> defaultShader;

        const char * vertexShader = 
            "#version " GLSL_VERSION "\n"
            "precision lowp float;\n"
            "in vec4 a_position;\n"
            "in vec4 a_offset;\n"
            "in vec4 a_colour;\n"
            "uniform mat4 proj;\n"
            "out vec2 texCoord;\n"
            "void main(void){"
                "vec2 pos = a_position.xy*a_offset.w;\n"
                "float ct = cos(a_offset.z); float st = sin(a_offset.z);\n"
                "mat2 rot = mat2(ct, -st, st, ct);\n"
                "pos = rot*pos + a_offset.xy;\n"
                "gl_Position = proj*vec4(pos,0.0,1.0);\n"
                "texCoord = a_position.zw;\n"
            "}";

        const char * fragmentShader = 
            "#version " GLSL_VERSION "\n"
            "precision lowp float;\n"
            "in vec2 texCoord;\n"
            "in vec4 colour;\n"
            "out vec4 fragment;\n"
            "void main(void){\n" 
                "fragment = colour;\n"
            "}";

    };
}

#endif /* GLSHAPERENDERER_H */
