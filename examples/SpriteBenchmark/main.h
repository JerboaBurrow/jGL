#ifndef MAIN_H
#define MAIN_H

#include <jGL/jGL.h>
#include <jGL/OpenGL/openGLInstance.h>

#include <logo.h>
#include <jGL/Display/desktopDisplay.h>
#include <jGL/orthoCam.h>

#include <jLog/jLog.h>

#include <rand.h>

#include <chrono>
using namespace std::chrono;

const int resX = 800;
const int resY = 1000;

uint8_t frameId = 0;
double deltas[60];

std::unique_ptr<jGL::jGLInstance> jGLInstance;

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

#endif /* MAIN_H */
