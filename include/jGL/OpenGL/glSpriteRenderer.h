#ifndef GLSPRITERENDERER
#define GLSPRITERENDERER

#include <jGL/OpenGL/Shader/glShader.h>
#include <jGL/OpenGL/gl.h>
#include <jGL/spriteRenderer.h>

namespace jGL::GL
{
    class glSpriteRenderer : public SpriteRenderer
    {
    public:

        glSpriteRenderer(size_t sizeHint)
        : SpriteRenderer(sizeHint)
        {
            offsets = std::vector<float>(sizeHint*offsetDim+padSprites*offsetDim,0.0f);
            textureRegion = std::vector<float>(sizeHint*textureRegionDim+padSprites*textureRegionDim,0.0f);
            textureOptions = std::vector<float>(sizeHint*textureOptionsDim+padSprites*textureOptionsDim,0.0f);
            initGL();
            defaultShader = std::make_shared<glShader>(vertexShader, fragmentShader);
            defaultShader->use();
        }

        ~glSpriteRenderer()
        {
            freeGL();
        }

        void draw(std::shared_ptr<Shader> shader, std::multimap<RenderPriority, SpriteId> ids);
        void draw(std::multimap<RenderPriority, SpriteId> ids) { draw(defaultShader, ids); }
        
    private:

        GLuint vao, a_position, a_offset, a_textureRegion, a_textureOption;

        float quad[6*4] = 
        {
            // positions  / texture coords
            0.5f,  0.5f, 1.0f, 1.0f,    // top right
            0.5f,  -0.5f, 1.0f, 0.0f,   // bottom right
            -0.5f,  -0.5f, 0.0f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f, 1.0f,   // top left 
            -0.5f,  -0.5f, 0.0f, 0.0f,  // bottom left
            0.5f,  0.5f, 1.0f, 1.0f     // top right
        };

        std::vector<float> offsets;         // offset x, y, theta, scale
        size_t offsetDim = 4;

        std::vector<float> textureRegion;  // tx, ty (region coords), lx, ly (region scale)
        size_t textureRegionDim = 4;
        
        std::vector<float> textureOptions;  // texture unit, alpha
        size_t textureOptionsDim = 2;

        size_t padSprites = 8;

        void initGL();
        void freeGL();

        std::shared_ptr<Shader> defaultShader;

        const char * vertexShader = 
            "#version " GLSL_VERSION "\n"
            "precision lowp float; precision lowp int;\n"
            "layout(location=0) in vec4 a_position;\n"
            "layout(location=1) in vec4 a_offset;\n"
            "layout(location=2) in vec4 a_textureOffset;\n"
            "layout(location=3) in vec2 a_textureOptions;\n"
            "uniform mat4 proj;\n"
            "out vec2 texCoord;\n"
            "flat out float alpha;\n"
            "flat out int tex;\n"
            "void main(){"
                "vec2 pos = a_position.xy*a_offset.w;\n"
                "float ct = cos(a_offset.z); float st = sin(a_offset.z);\n"
                "mat2 rot = mat2(ct, -st, st, ct);\n"
                "pos = rot*pos + a_offset.xy;\n"
                "gl_Position = proj*vec4(pos,0.0,1.0);\n"
                "texCoord.x = (a_position.z * a_textureOffset.z)+a_textureOffset.x;\n"
                "texCoord.y = (a_position.w * a_textureOffset.w)+a_textureOffset.y;\n"
                "tex = int(a_textureOptions.x);\n"
                "alpha = a_textureOptions.y;\n"
            "}";

        const char * fragmentShader = 
            "#version " GLSL_VERSION "\n"
            "precision lowp float; precision lowp int;\n"
            "uniform sampler2D sampler0;\n"
            "uniform sampler2D sampler1;\n"
            "uniform sampler2D sampler2;\n"
            "uniform sampler2D sampler3;\n"
            "in vec2 texCoord;\n"
            "flat in float alpha;\n"
            "flat in int tex;\n"
            "layout(location=0) out vec4 colour;\n"
            "void main(){\n" 
                // is this mental?
                "if (tex == 0) {colour = texture(sampler0, texCoord);}\n"
                "else if (tex == 1) {colour = texture(sampler1, texCoord);}\n"
                "else if (tex == 2) {colour = texture(sampler2, texCoord);}\n"
                "else if (tex == 3) {colour = texture(sampler3, texCoord);}\n"
                "else {colour = vec4(0.0,0.0,0.0,alpha);}\n"
                "colour.a = colour.a*alpha;\n"
                ";\n"
            "}";

    };
}

#endif /* GLSPRITERENDERER */
