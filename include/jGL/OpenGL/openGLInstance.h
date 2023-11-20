#ifndef OPENGLINSTANCE
#define OPENGLINSTANCE

#include <jGL/jGL.h>
#include <jGL/OpenGL/Text/textRenderer.h>
#include <jGL/OpenGL/glParticles.h>
#include <jGL/OpenGL/glSpriteRenderer.h>
#include <jGL/OpenGL/Texture/glTexture.h>
#include <jGL/OpenGL/glDrawFramebuffer.h>

namespace jGL::GL
{

    class glParticles;

    class OpenGLInstance : public jGLInstance
    {
        
        public:

            OpenGLInstance(const Display & display)
            : jGLInstance(display), framebuffer(glDrawFramebuffer())
            {
                framebuffer.setResolution
                (
                    glm::vec2
                    (
                        display.getResX(), 
                        display.getResY()
                    )
                );
                framebuffer.setMSAA(1);
            }

            ~OpenGLInstance(){framebuffer.destroy();}

            void beginFrame(){framebuffer.bind();}
            void endFrame(){framebuffer.blit();}

            void text
            (
                std::string characters, 
                glm::vec2 position,
                float scale,
                glm::vec4 colour
            );

            void clear()
            {
                glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                framebuffer.setClear(clearColour);
            }

            void setMSAA(uint8_t samples)
            {
                GLint maxSamples;
                glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

                if (samples > maxSamples){samples = maxSamples;}

                framebuffer.setMSAA(samples);
            }

            void setClear(glm::vec4 colour) { clearColour = colour; }
            void setProjection(glm::mat4 proj) {/*TODO*/}
            void setTextProjection(glm::mat4 proj) { textRenderer.setProjection(proj); }
            
            std::shared_ptr<Particles> createParticles(size_t sizeHint) 
            {
                return std::static_pointer_cast<Particles>(std::make_shared<glParticles>(sizeHint)); 
            }

            std::shared_ptr<Texture> createTexture(std::string imageFile, Texture::Type type) 
            { 
                switch (type)
                {
                    case Texture::Type::RGB:
                        return std::static_pointer_cast<Texture>(std::make_shared<glTexture2DRGB>(imageFile));
                    case Texture::Type::RGBA:
                        return std::static_pointer_cast<Texture>(std::make_shared<glTexture2DRGBA>(imageFile));
                    default:
                        return std::static_pointer_cast<Texture>(std::make_shared<glTexture2DRGB>(imageFile));
                }   
                
            }

            std::shared_ptr<SpriteRenderer> createSpriteRenderer(size_t sizeHint)
            {
                return std::static_pointer_cast<SpriteRenderer>
                (
                    std::make_shared<glSpriteRenderer>
                    (
                        sizeHint
                    )
                );
            }

        private:

            glDrawFramebuffer framebuffer;

            Type defaultFont = Type(48);
            TextRenderer textRenderer;

            glm::vec4 clearColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    };
}

#endif /* OPENGLINSTANCE */
