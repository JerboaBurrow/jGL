#ifndef MAIN
#define MAIN

#include <jGL/jGL.h>
#include <jGL/OpenGL/openGLInstance.h>

#include <logo.h>
#include <jGL/Display/display.h>
#include <jGL/orthoCam.h>

#include <jLog/jLog.h>
#include <Util/util.h>

#include <chrono>
using namespace std::chrono;

const int resX = 800;
const int resY = 1000;

uint8_t frameId = 0;
double deltas[60];

std::unique_ptr<jGL::jGLInstance> jGLInstance;

using Particles::Util::fixedLengthNumber;

#endif /* MAIN */
