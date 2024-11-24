#include <jGL/OpenGL/gl.h>
#include <iostream>

namespace jGL::GL
{
    // print buffer status errors
    GLuint glBufferStatus(const std::string msg)
    {
        GLuint e = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        switch(e)
        {
            case GL_FRAMEBUFFER_UNDEFINED:
            throw( GLRuntimeException(msg+" GLERROR: GL_FRAMEBUFFER_UNDEFINED\n") );
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                throw( GLRuntimeException(msg+" GLERROR: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n") );
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                throw( GLRuntimeException(msg+" GLERROR: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n") );
                break;

            case GL_FRAMEBUFFER_UNSUPPORTED:
                throw( GLRuntimeException(msg+" GLERROR: GL_FRAMEBUFFER_UNSUPPORTED\n") );
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                throw( GLRuntimeException(msg+" GLERROR: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n") );
                break;
        }
        return e;
    }

    // print gl error codes
    GLuint glError(const std::string msg)
    {
        GLuint e = glGetError();
        switch(e)
        {
            case GL_NO_ERROR:
            break;
            case GL_INVALID_ENUM:
                throw( GLRuntimeException(msg+" GL_INVALID_ENUM\n") );
                break;
            case GL_INVALID_VALUE:
                throw( GLRuntimeException(msg+" GLERROR: GL_INVALID_VALUE\n") );
                break;
            case GL_INVALID_OPERATION:
                throw( GLRuntimeException(msg+" GLERROR: GL_INVALID_OPERATION\n") );
                break;
            case GL_OUT_OF_MEMORY:
                throw( GLRuntimeException(msg+" GLERROR: GL_OUT_OF_MEMORY\n") );
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                throw( GLRuntimeException(msg+" GLERROR: GL_INVALID_FRAMEBUFFER_OPERATION\n") );
                break;
        }
        return e;
    }

}