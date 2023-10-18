#ifndef VKPARTICLES
#define VKPARTICLES

#include <jGL/particles.h>

namespace jGL::Vulkan
{
    class vkParticles : public Particles
    {
    public:

        vkParticles(size_t sizeHint)
        : Particles(sizeHint)
        {}

        void update(Particles::UpdateInfo info) {/*TODO*/}

        void draw(glm::mat4 proj) {/*TODO*/}

        void setTexture(std::shared_ptr<Texture>) {/*TODO*/}
    };
}

#endif /* VKPARTICLES */
