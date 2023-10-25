#ifndef TOY_H
#define TOY_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace Particles 
{
    enum class TOY_TYPE {ATTRACTOR, REPELLER, SPINNER};

    class Toy
    /*
        A toy is a position, and a TOY_TYPE, there can be 8 of them (packed into a mat4, i.e 16 floats)
    */
    {

    public:

        Toy(TOY_TYPE t)
        : next(0), type(t)
        {
            positions = std::vector<float>(16, 0.0f);   
        }

        const size_t getCount() const {return next;}

        void addToy(float x, float y)
        {
            if (next < 8)
            {
                positions[next*2] = x;
                positions[next*2+1] = y;
                next++;
            }
        }

        void removeToy(size_t n)
        {
            positions[n*2] = -100.0f;
            positions[n*2+1] = -100.0f;

            // shift all down a peg
            if (n < next-1)
            {
                for (unsigned i = n; n < next-1; n++)
                {
                    positions[i*2] = positions[(i+1)*2];
                    positions[i*2+1] = positions[(i+1)*2+1];
                }
            }
        }

        glm::mat4 asMat4() {return glm::make_mat4(&positions[0]);}

    private:

        std::vector<float> positions;
        size_t next;
        TOY_TYPE type;

    };
}

#endif /* TOY_H */