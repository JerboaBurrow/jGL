#ifndef MAIN
#define MAIN

#include <jGL/jGL.h>
#include <jGL/OpenGL/openGLInstance.h>
#include <jGL/Vulkan/vulkanInstance.h>

#include <logo.h>
#include <jGL/Display/desktopDisplay.h>
#include <jGL/orthoCam.h>

#include <dynamics.h>
#include <jLog/jLog.h>

#include <chrono>
using namespace std::chrono;

const int resX = 800;
const int resY = 1000;

const float SCALE_PX = 16.0f;

const size_t nParticles = 1;

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

void keyEventCallback
(
    GLFWwindow * window,
    int key,
    int scancode,
    int action,
    int mods
)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        jGLInstance->finish();
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

#endif /* MAIN */
