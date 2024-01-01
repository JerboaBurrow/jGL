#include <jGL/OpenGL/glParticles.h>

namespace jGL::GL
{

    void glParticles::initGL()
    {

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &a_position);
        glGenBuffers(1, &a_colour);
        glGenBuffers(1, &a_texCoord);

        glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, a_position);
                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    4*sizeof(float)*points.size(),
                    position.data(),
                    GL_DYNAMIC_DRAW
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
                glVertexAttribDivisor(0, 1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_colour);
                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    4*sizeof(float)*points.size(),
                    colour.data(),
                    GL_DYNAMIC_DRAW
                );
                glEnableVertexAttribArray(1);
                glVertexAttribPointer
                (
                    1,
                    4,
                    GL_FLOAT,
                    false,
                    4*sizeof(float),
                    0
                );
                glVertexAttribDivisor(1, 1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, a_texCoord);
                glBufferData
                (
                    GL_ARRAY_BUFFER,
                    4*sizeof(float)*points.size(),
                    texCoord.data(),
                    GL_DYNAMIC_DRAW
                );
                glEnableVertexAttribArray(2);
                glVertexAttribPointer
                (
                    2,
                    4,
                    GL_FLOAT,
                    false,
                    4*sizeof(float),
                    0
                );
                glVertexAttribDivisor(2, 1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        uploaded = points.size();
        drawing = uploaded;

        shader = glShader(vertexShader, fragmentShader);
        shader.compile();

        glError("glParticles init");

    }

    void glParticles::flatten(UpdateInfo info)
    {
        for (unsigned i = 0; i < points.size(); i++)
        {
            if (info.position)
            {
                glm::vec4 & state = points[i].state;
                position[i*4] = state.x;
                position[i*4+1] = state.y;
                position[i*4+2] = state.z;
                position[i*4+3] = state.w;
            }

            if (info.colour)
            {
                glm::vec4 & state = points[i].colour;
                colour[i*4] = state.x;
                colour[i*4+1] = state.y;
                colour[i*4+2] = state.z;
                colour[i*4+3] = state.w;
            }

            if (info.texCoord)
            {
                glm::vec4 & state = points[i].texCoord;
                texCoord[i*4] = state.x;
                texCoord[i*4+1] = state.y;
                texCoord[i*4+2] = state.z;
                texCoord[i*4+3] = state.w;
            }
        }
    }

    glParticles::glParticles(size_t sizeHint)
    : Particles(sizeHint)
    {
        position.resize(sizeHint*4);
        colour.resize(sizeHint*4);
        texCoord.resize(sizeHint*4);

        initGL();
    }

    glParticles::glParticles(std::vector<TexturedParticle> p)
    : Particles(p)
    {
        flatten(Particles::UpdateInfo {true, true, true});
        initGL();
    }

    void glParticles::update(UpdateInfo info)
    {

        if (uploaded < points.size())
        {
            position.resize(points.size()*4);
            colour.resize(points.size()*4);
            texCoord.resize(points.size()*4);

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &a_position);
            glGenBuffers(1, &a_colour);
            glGenBuffers(1, &a_texCoord);

            info.colour = true;
            info.position = true;
            info.texCoord = true;

            initGL();
        }
        else if (uploaded > points.size()) 
        {
            drawing = points.size();
        }

        flatten(info);

        glBindVertexArray(vao);

            if (info.position)
            {
                glBindBuffer(GL_ARRAY_BUFFER, a_position);
                    glBufferSubData
                    (
                        GL_ARRAY_BUFFER,
                        0,
                        sizeof(float)*position.size(),
                        position.data()
                    );
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            if (info.colour)
            {
                glBindBuffer(GL_ARRAY_BUFFER, a_colour);
                    glBufferSubData
                    (
                        GL_ARRAY_BUFFER,
                        0,
                        sizeof(float)*colour.size(),
                        colour.data()
                    );
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            if (info.texCoord)
            {
                glBindBuffer(GL_ARRAY_BUFFER, a_texCoord);
                    glBufferSubData
                    (
                        GL_ARRAY_BUFFER,
                        0,
                        sizeof(float)*texCoord.size(),
                        texCoord.data()
                    );
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

        glBindVertexArray(0);
    }

    void glParticles::draw(glm::mat4 proj)
    {
        #ifndef ANDROID // IFLCPD
        glEnable(GL_PROGRAM_POINT_SIZE);
        #endif
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        shader.setUniform<glm::mat4>("proj", proj);
        shader.setUniform<Sampler2D>("sampler", Sampler2D(0));
        shader.setUniform<float>("scale", baseLineScalePixels);

        if (texture == nullptr)
        {
            shader.setUniform<int>("textureless", 1);
        }
        else
        {
            shader.setUniform<int>("textureless", 0);
            texture->bind(0);
        }

        glBindVertexArray(vao);

            shader.use();
            glDrawArraysInstanced(GL_POINTS, 0, 1, drawing);

        glBindVertexArray(vao);

        glError("glParticles draw");
    }

}