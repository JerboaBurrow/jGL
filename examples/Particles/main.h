#ifndef MAIN
#define MAIN

#include <jGL/jGL.h>
#include <jGL/OpenGL/openGLInstance.h>
#include <jGL/Vulkan/vulkanInstance.h>

#include <logo.h>
#include <jGL/Display/display.h>
#include <jGL/orthoCam.h>

#include <dynamics.h>
#include <log.h>
#include <Util/util.h>

#include <chrono>
using namespace std::chrono;

const int resX = 800;
const int resY = 1000;

const float SCALE_PX = 16.0f;

const size_t nParticles = 1024;

uint8_t frameId = 0;
double deltas[60];

std::unique_ptr<jGL::jGLInstance> jGLInstance;

using Particles::Util::fixedLengthNumber;

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
