#ifndef GLTEXTURE
#define GLTEXTURE

#include <jGL/OpenGL/gl.h>
#include <jGL/texture.h>

#include <stdexcept>

namespace jGL::GL
{

    class glTexture : public Texture
    {

    public:

        glTexture()
        {
            glGenTextures(1, &id);
        }

        ~glTexture()
        {
            glDeleteTextures(1, &id);
        }


        inline void bind() { glBindTexture(GL_TEXTURE_2D, id); }

        inline void unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

        void bind(unsigned binding) 
        {
            glActiveTexture(GL_TEXTURE0+binding);
            bind();
            textureUnit = binding;
        }

        void generateMipMaps() {bind(); glGenerateMipmap(GL_TEXTURE_2D);}

    protected:

        GLuint id;
        unsigned textureUnit;

    };

    class glTexture2DRGB : public glTexture
    {

    public:

        glTexture2DRGB(std::string imageFile);

        void upload(float * data);
    };

    class glTexture2DRGBA : public glTexture
    {

    public:

        glTexture2DRGBA(std::string imageFile);

        void upload(float * data);
    };
}

#endif /* GLTEXTURE */
