#include <Util/util.h>

namespace Particles::Util
{
    // forces the number x to be rendered in exactly length characters as a string
    std::string fixedLengthNumber(double x, unsigned length)
    {
    std::string d = std::to_string(x);
    std::string dtrunc(length,' ');
    for (unsigned c = 0; c < dtrunc.length(); c++/*ayy lmao*/)
    {

        if (c >= d.length())
        {
            dtrunc[c] = '0';
        }
        else
        {
            dtrunc[c] = d[c];
        }
    }
    return dtrunc;
    }

}