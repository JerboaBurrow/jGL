#include <dynamics.h>
#include <iostream>
namespace Particles::Dynamics
{

    void ParticleSystem::initParticles(std::vector<TexturedParticle> & particles)
    {
        lastParticleStates = std::vector<float>(particles.size()*4, 0.0f);

        float x, y, theta, s;

        float ax = bounds.lowX+scale;
        float ay = bounds.lowY+scale;
        float bx = bounds.highX-scale;
        float by = bounds.highY-scale;

        float mx = (bx-ax)+ax;
        float my = (by-ay)+ay;

        for (unsigned i = 0; i < particles.size(); i++)
        {
            x = rng.nextFloat()*mx;
            y = rng.nextFloat()*my;
            theta = rng.nextFloat()*2.0f*3.14159;
            s = rng.nextFloat()*(1.0f-0.25f)+0.25f;

            particles[i].state = glm::vec4(x, y, theta, s);

            lastParticleStates[i*4] = x;
            lastParticleStates[i*4+1] = y;
            lastParticleStates[i*4+2] = theta;
            lastParticleStates[i*4+3] = rng.nextFloat()*DR;
        }
    }

    void ParticleSystem::stepParticles
    (
        std::vector<TexturedParticle>::iterator from,
        std::vector<TexturedParticle>::iterator to,
        unsigned offset
    )
    {
        // too much fortran
        float nx, ny, nt, px, py, fx, fy, tau, dc2x, dc2y, dc2, rx, ry, r, dtdt;

        float cx = (bounds.highX - bounds.lowX)/2.0f;
        float cy = (bounds.highY - bounds.lowY)/2.0f;

        unsigned n = std::distance(from, to);
        unsigned i = offset+n;

        dtdt = dt*dt;
        // decrement to reduce false sharing probability 
        //  as state is also accessed in jGL (main) thread
        auto iter = to;
        while(iter != from)
        {
            iter--;
            i--;

            glm::vec4 & state = iter->state;

            dc2x = cx - state.x;
            dc2y = cy - state.y;

            dc2 = (dc2x * dc2x + dc2y * dc2y) / scale;

            if (dc2 > 10.0) {dc2 = 10.0;}

            // self propulsion along theta
            px = std::cos(state.z);
            py = std::sin(state.z);

            fx = 10.0f * scale * px;
            fy = 10.0f * scale * py;

            rx = (cx - state.x);
            ry = (cy - state.y);

            r = std::sqrt(rx*rx+ry*ry);

            // diffuse plus rotate to centre
            tau = rng.nextFloat()*DR - dc2*(rx * py - ry * px)/r;

            float & lx = lastParticleStates[i*4];
            float & ly = lastParticleStates[i*4+1];
            float & lt = lastParticleStates[i*4+2];

            nx = 2.0f*state.x - lx + dtdt * fx;
            ny = 2.0f*state.y - ly + dtdt * fy;
            nt = 2.0f*state.z - lt + dtdt * tau;

            lx   = state.x;
            ly = state.y;
            lt = state.z;

            state.x = nx;
            state.y = ny;
            state.z = nt;
            
        }
    }

    void ParticleSystem::step(std::vector<TexturedParticle> & particles)
    {

        if (4*particles.size() != lastParticleStates.size())
        {
            initParticles(particles);
        }
        // do not block, physics won't go unstable
        workers.queueJob
        (
            std::bind
            (
                &ParticleSystem::stepParticles,
                this,
                particles.begin(),
                particles.end(),
                0
            )
        );
    }

}