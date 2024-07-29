#ifndef GLTEXTURE_H
#define GLTEXTURE_H

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

        virtual void upload(unsigned char * data) {};

        inline void bind() { glBindTexture(GL_TEXTURE_2D, id); }

        inline void unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

        void bind(unsigned binding) 
        {
            glActiveTexture(GL_TEXTURE0+binding);
            unbind();
            bind();
            textureUnit = binding;
        }

        void generateMipMaps() {bind(); glGenerateMipmap(GL_TEXTURE_2D);}

    protected:

        GLuint id;
        unsigned textureUnit;

        std::vector<unsigned char> load_image(std::string path)
        {
            unsigned char * pixels = stbi_load(path.c_str(), &width, &height, &channels, 0);
        
            if (!pixels)
            {
                throw std::runtime_error("Failed to load texture: "+path);
            }

            size_t dim = width*height;
            if (channels > 0) { dim *= channels; }
            std::vector<unsigned char> vdata(pixels, pixels+dim);
            stbi_image_free(pixels);
            return vdata;
        }

        std::vector<unsigned char> load_image(std::vector<unsigned char> memory)
        {
            unsigned char * pixels = stbi_load_from_memory(memory.data(), memory.size(), &width, &height, &channels, 4);
            if (!pixels)
            {
                throw std::runtime_error("Failed to load texture from memory");
            }

            size_t dim = width*height;
            if (channels > 0) { dim *= channels; }
            std::vector<unsigned char> vdata(pixels, pixels+dim);
            stbi_image_free(pixels);
            return vdata;
        }

    };

    class glTexture2DRGB : public glTexture
    {

    public:

        glTexture2DRGB(std::string imageFile)
        : glTexture()
        {
            std::vector<unsigned char> pixels = load_image(imageFile);
            create(width, height, channels);
            upload(pixels);
        }

        glTexture2DRGB(std::vector<unsigned char> data)
        : glTexture()
        {
            std::vector<unsigned char> pixels = load_image(data);
            create(width, height, channels);
            upload(pixels);
        }

        void create(int width, int height, int channels);
        void upload(std::vector<unsigned char> pixels);
    };

    class glTexture2DRGBA : public glTexture
    {

    public:

        glTexture2DRGBA(std::string imageFile)
        : glTexture()
        {
            std::vector<unsigned char> pixels = load_image(imageFile);
            create(width, height, channels);
            upload(pixels);
        }
    
        glTexture2DRGBA(std::vector<unsigned char> data)
        : glTexture()
        {
            std::vector<unsigned char> pixels = load_image(data);
            create(width, height, channels);
            upload(pixels);
        }

        void create(int width, int height, int channels);
        void upload(std::vector<unsigned char> pixels);
    };

    class glTexture2DByte : public glTexture
    {

    public:

        glTexture2DByte(std::string imageFile)
        : glTexture()
        {
            std::vector<unsigned char> pixels = load_image(imageFile);
            create(width, height);
            upload(pixels);
        }

        glTexture2DByte(std::vector<unsigned char> data)
        : glTexture()
        {
            std::vector<unsigned char> pixels = load_image(data);
            create(width, height);
            upload(pixels);
        }

        glTexture2DByte(std::vector<unsigned char> pixels, int width, int height)
        : glTexture()
        {
            create(width, height);
            upload(pixels);
        }

        void create(int width, int height);
        void upload(std::vector<unsigned char> pixels);
        
    };
}

#endif /* GLTEXTURE_H */
