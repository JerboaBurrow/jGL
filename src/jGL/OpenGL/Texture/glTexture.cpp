#include <jGL/OpenGL/Texture/glTexture.h>

namespace jGL::GL
{
    void glTexture2DRGB::create(int width, int height, int channels)
    {
        this->width = width;
        this->height = height;
        this->channels = channels;
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

        glBindTexture(GL_TEXTURE_2D, 0);

    }

    void glTexture2DRGB::upload(std::vector<std::byte> pixels)
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
            pixels.data()
        );
    }

    void glTexture2DRGBA::create(int width, int height, int channels)
    {
        this->width = width;
        this->height = height;
        this->channels = channels;
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

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void glTexture2DRGBA::upload(std::vector<std::byte> pixels)
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
            pixels.data()
        );
    }

    void glTexture2DByte::create(int width, int height)
    {
        this->width = width;
        this->height = height;
        
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
            GL_R8,
            width,
            height,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            NULL
        );

        glBindTexture(GL_TEXTURE_2D, 0);

    }

    void glTexture2DByte::upload(std::vector<std::byte> pixels)
    {
        bind();

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R8,
            width,
            height,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            pixels.data()
        );
    }

}