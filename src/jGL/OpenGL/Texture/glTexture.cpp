#include <jGL/OpenGL/Texture/glTexture.h>

namespace jGL::GL
{

    glTexture2DRGB::glTexture2DRGB(std::string imageFile)
    : glTexture()
    {
        unsigned char * data = stbi_load(imageFile.c_str(), &width, &height, &channels, 0);
        
        if (!data)
        {
            throw std::runtime_error("Failed to load texture: "+imageFile);
        }
        
        bind();

        glTexParameteri
        (
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_NEAREST
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_NEAREST
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_S,
            GL_CLAMP_TO_EDGE
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_T,
            GL_CLAMP_TO_EDGE
        );

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            NULL
        );

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            data
        );

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);

    }

    void glTexture2DRGB::upload(float * data)
    {
        bind();

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            data
        );
    }

    glTexture2DRGBA::glTexture2DRGBA(std::string imageFile)
    : glTexture()
    {
        unsigned char * data = stbi_load(imageFile.c_str(), &width, &height, &channels, 0);
        
        if (!data)
        {
            throw std::runtime_error("Failed to load texture: "+imageFile);
        }
        
        bind();

        glTexParameteri
        (
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_NEAREST
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_NEAREST
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_S,
            GL_CLAMP_TO_EDGE
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_T,
            GL_CLAMP_TO_EDGE
        );

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            NULL
        );

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data
        );

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);

    }

    void glTexture2DRGBA::upload(float * data)
    {
        bind();

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data
        );
    }

}