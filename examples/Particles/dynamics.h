#ifndef COMPUTE_H
#define COMPUTE_H

#include <jGL/particles.h>

#include <threadPool.h>

#include <toy.h>
#include <rand.h>
#include <math.h>
#include <iterator>

namespace Particles::Dynamics
{

    using Particles::Toy;
    using Particles::TOY_TYPE;
    using Particles::Random::RNG;

    using jGL::Particles;
    using jGL::TexturedParticle;

    struct Bounds 
    {
        Bounds(float lx, float hx, float ly, float hy)
        : lowX(lx), highX(hx), lowY(ly), highY(hy)
        {}

        float lowX;
        float highX;
        float lowY;
        float highY;
    };

    class ParticleSystem
    {

    public:

        ParticleSystem
        (
            Bounds b,
            float s
        )
        : bounds(b),
          scale(s),
          attractors(TOY_TYPE::ATTRACTOR),
          repellers(TOY_TYPE::REPELLER),
          spinners(TOY_TYPE::SPINNER),
          workers(1)
        {}

        void setBounds(Bounds b){ bounds = b; }

        void step(std::vector<TexturedParticle> & particles);

        void sync() {workers.wait();}

    protected:

        Bounds bounds;

        float scale = 0.01f;

        std::vector<float> lastParticleStates;

        Toy attractors;
        Toy repellers;
        Toy spinners;

        ThreadPool workers;

    private:

        float dt = 1.0/60.0;

        float M = 0.001;
        float J = 0.01;
        float dr = 1.0/60.0f;
        float DR = std::sqrt(2.0*0.01*dt);

        float cr = (1.0*dt)/(2.0*J);
        float br = 1.0 / (1.0+cr);
        float ar = (1.0-cr)*br;

        float ct = (1.0*dt)/(2.0*M);
        float bt = 1.0 / (1.0+ct);
        float at = (1.0-ct)*bt;

        float alpha = bt*dt*dt/M;
        float beta = br*dt*dt/J;
        float gamma = br*dt/(2.0*J);

        RNG rng;

        void initParticles(std::vector<TexturedParticle> & particles, bool clean = true);

        void stepParticles
        (
            std::vector<TexturedParticle>::iterator from,
            std::vector<TexturedParticle>::iterator to,
            unsigned offset = 0
        );

    };

}

#endif /* COMPUTE_H */
