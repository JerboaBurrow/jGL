#include <stdint.h>
#include <iostream>
#include <random>
const double tol = 1e-6;
#include <cmath>

#include <jGL/OpenGL/Shader/glShader.h>
#include <jGL/Display/desktopDisplay.h>
#include <jGL/common.h>
#include <stb_image.h>

std::default_random_engine e;
std::uniform_int_distribution<uint64_t> U(0,-1);

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using jGL::Sampler2D;
using jGL::GL::glShader;
using jGL::jGLUniform;
using jGL::AbstractjGLUniform;
using jGL::DesktopDisplay;

static const char * TESTVS01 =
    "#version 300 es\n"
    "precision highp float;\n"
    "precision highp int;\n"
    "layout(location = 0) in vec3 a_position;\n"
    "layout(location = 1) in vec2 a_texCoords;\n"
    "out vec2 o_texCoords;\n"
    "void main(void){\n"
    "   gl_Position = vec4(a_position,1);\n"
    "   o_texCoords = a_texCoords.st;\n"
    "}\n";

static const char * TESTFS01 =
        "#version 300 es\n"
        "precision highp float;\n"
        "precision highp int;\n"
        "out vec4 colour;\n"
        "void main(void) {\n"
            "colour = vec4(1.0,1.0,1.0,1.0);\n"
        "}";

static const char * TESTVS02 =
    "#version 300 es\n"
    "precision highp float;\n"
    "precision highp int;\n"
    "layout(location = 0) in vec3 a_position;\n"
    "layout(location = 1) in vec2 a_texCoords;\n"
    "out vec2 o_texCoords;\n"
    "uniform int x; uniform float y;\n"
    "void main(void){\n"
    "   gl_Position = vec4(a_position,1);\n"
    "   o_texCoords = a_texCoords.st;\n"
    "}\n";

static const char * TESTFS02 =
        "#version 300 es\n"
        "precision highp float;\n"
        "precision highp int;\n"
        "out vec4 colour;\n"
        "uniform mat4 z; uniform vec2 w;\n"
        "void main(void) {\n"
            "colour = vec4(1.0,1.0,1.0,1.0);\n"
        "}";

static const char * TESTVS03 =
    "#version 300 es\n"
    "precision highp float;\n"
    "precision highp int;\n"
    "layout(location = 0) in vec3 a_position;\n"
    "layout(location = 1) in vec2 a_texCoords;\n"
    "out vec2 o_texCoords;\n"
    "void main(void){\n"
    "   gl_Position = vec4(a_position,1);\n"
    "   o_texCoords = a_texCoords.st;\n"
    "}\n";

static const char * TESTFS03 =
        "#version 300 es\n"
        "precision highp float;\n"
        "precision highp int;\n"
        "out vec4 colour;\n"
        "uniform lowp sampler2D texture;\n"
        "uniform sampler2D noPrecisionTexture;\n"
        "void main(void) {\n"
            "colour = vec4(1.0,1.0,1.0,1.0);\n"
        "}";


SCENARIO("Shader uniforms", "[shaders]")
{
    GIVEN("The test shaders TESTFS01 and TESTVS01")
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        GLFWwindow * glfwWindow = glfwCreateWindow(1,1,"Particles: Tests",NULL,NULL); 
        glfwSwapInterval(1);
        glfwMakeContextCurrent(glfwWindow);
        glewInit();
        glShader s(TESTVS01, TESTFS01);
        WHEN("A Shader, s,  is instantiated")
        {
            THEN("s is not a program")
            {
                REQUIRE(s.isProgram() == false);
            }
            THEN("s is not compiled")
            {
                REQUIRE(s.isCompiled() == false);
            }
            THEN("s has no uniforms")
            {
                REQUIRE(s.getUniformNames().size() == 0);
            }
        }
        glfwDestroyWindow(glfwWindow); 
        glfwTerminate();
    }

    GIVEN("The test shaders TESTFS02 and TESTVS02")
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        GLFWwindow * glfwWindow = glfwCreateWindow(1,1,"Particles: Tests",NULL,NULL); 
        glfwSwapInterval(1);
        glfwMakeContextCurrent(glfwWindow);
        glewInit();
        glShader s(TESTVS02, TESTFS02);
        WHEN("A Shader, s, is instantiated")
        {
            THEN("s has 4 uniforms")
            {
                REQUIRE(s.getUniformNames().size() == 4);
            }

            THEN("s has an int uniform x")
            {
                jGLUniform<int> x = s.getUniform<int>("x");
                AND_THEN("x has name \"x\" and value 0")
                {
                    REQUIRE(x.name == "x");
                    REQUIRE(x.value == 0);
                }
            }

            THEN("s has a float uniform y")
            {
                jGLUniform<float> y = s.getUniform<float>("y");
                AND_THEN("y has name \"y\" and value 0.0f")
                {
                    REQUIRE(y.name == "y");
                    REQUIRE(y.value == 0.0f);
                }
            }

            THEN("s has an glm::mat4 uniform z")
            {
                jGLUniform<glm::mat4> z = s.getUniform<glm::mat4>("z");
                AND_THEN("z has name \"z\" and value glm::mat4(0.0f)")
                {
                    REQUIRE(z.name == "z");
                    REQUIRE(z.value == glm::mat4(0.0f));
                }
            }

            THEN("s has a glm::vec2 uniform w")
            {
                jGLUniform<glm::vec2> w = s.getUniform<glm::vec2>("w");
                AND_THEN("w has name \"w\" and value glm::vec2(0.0f)")
                {
                    REQUIRE(w.name == "w");
                    REQUIRE(w.value == glm::vec2(0.0f));
                }
            }

            WHEN("Setting y to 3.14159")
            {
                s.setUniform<float>("y",3.14159f);
                THEN("y has the value 3.14159");
                {
                    REQUIRE(s.getUniform<float>("y").value == 3.14159f);
                }
            }

            WHEN("s is compiled")
            {
                s.compile();
                THEN("s is a program")
                {
                    REQUIRE(s.isProgram());
                }
                WHEN("Setting y to 1.4142")
                {
                    s.setUniform<float>("y",1.4142f);
                    THEN("y has the value 1.4142");
                    {
                        REQUIRE(s.getUniform<float>("y").value == 1.4142f);
                    }
                }
                WHEN("Setting w to vec2(1.0f, -1.0f)")
                {
                    s.setUniform<glm::vec2>("w", glm::vec2(1.0f, -1.0f));
                    THEN("w has the value vec2(1.0f, -1.0f)")
                    {
                        REQUIRE(s.getUniform<glm::vec2>("w").value.x == 1.0f);
                        REQUIRE(s.getUniform<glm::vec2>("w").value.y == -1.0f);
                    }
                }
                
            }

            WHEN("s is released")
            {
                s.release();
                THEN("s is not a program")
                {
                    REQUIRE(!s.isProgram());
                }
            }
        }
        glfwDestroyWindow(glfwWindow); 
        glfwTerminate();
    }

    GIVEN("The test shaders TESTFS03 and TESTVS03")
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        GLFWwindow * glfwWindow = glfwCreateWindow(1,1,"Particles: Tests",NULL,NULL); 
        glfwSwapInterval(1);
        glfwMakeContextCurrent(glfwWindow);
        glewInit();
        glShader s(TESTVS03, TESTFS03);
        WHEN("A Shader, s, is instantiated")
        {
            THEN("s has 2 uniform")
            {
                REQUIRE(s.getUniformNames().size() == 2);
            }

            THEN("s has a sampler2D uniform texture")
            {
                jGLUniform<Sampler2D> x = s.getUniform<Sampler2D>("texture");
                AND_THEN("texture has name \"texture\" and value.texture = 0")
                {
                    REQUIRE(x.name == "texture");
                    REQUIRE(x.value.texture == 0);
                }
            }
            THEN("s has a sampler2D uniform noPrecisionTexture")
            {
                jGLUniform<Sampler2D> x = s.getUniform<Sampler2D>("noPrecisionTexture");
                AND_THEN("noPrecisionTexture has name \"texture\" and value.texture = 0")
                {
                    REQUIRE(x.name == "noPrecisionTexture");
                    REQUIRE(x.value.texture == 0);
                }
            }
        }
        glfwDestroyWindow(glfwWindow); 
        glfwTerminate();
    }
}

