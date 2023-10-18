#include <main.h>

int main(int argv, char ** argc)
{

    bool vulkan = false;

    if (argv > 1)
    {
        if (std::string(argc[1]) == "vk")
        {
            vulkan = true;
        }
    }

    jGL::Display display(resX, resY, "Particles", vulkan);

    glewInit();
    
    if (vulkan)
    {
        jGLInstance = std::move(std::make_unique<jGL::Vulkan::VulkanInstance>(display));
    }
    else
    {
        jGLInstance = std::move(std::make_unique<jGL::GL::OpenGLInstance>(display));
    }

    jGL::OrthoCam camera(resX, resY, glm::vec2(0.0,0.0));

    camera.setPosition(0.0f, 0.0f);

    glm::vec2 res = camera.getResolution();
    glm::vec4 minWorld = camera.screenToWorld(0.0f, 0.0f);
    glm::vec4 maxWorld = camera.screenToWorld(res.x, res.y);

    float scale = SCALE_PX / std::max(resX, resY);

    Log log;

    high_resolution_clock::time_point tic, tock, rdt0, rut0, pt0;
    double rdt, rut, pt;

    jGLInstance->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));

    std::shared_ptr<jGL::Particles> jGLParticles = jGLInstance->createParticles(nParticles);
    std::vector<jGL::TexturedParticle> & jGLParticleStates = jGLParticles->getParticles();
    jGLParticles->setBaseLineScale(SCALE_PX);

    for (unsigned i = 0; i < nParticles; i++)
    {
        jGLParticleStates[i].state = glm::vec4(0.0f);
        jGLParticleStates[i].colour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        jGLParticleStates[i].texCoord = glm::vec4(0.0f);
    }

    Particles::Dynamics::ParticleSystem particles
    (
        Particles::Dynamics::Bounds(minWorld.x, maxWorld.x, maxWorld.y, minWorld.y),
        scale
    );

    std::shared_ptr<jGL::Texture> saturn = jGLInstance->createTexture
    (
        "resource/texture/saturn.png",
        jGL::Texture::Type::RGBA
    );

    saturn->bind(0);
    jGLParticles->setTexture(saturn);

    double delta = 0.0;

    while (display.isOpen())
    {
        tic = high_resolution_clock::now();

        jGLInstance->clear();

        pt0 = high_resolution_clock::now();

        particles.step(jGLParticleStates);

        pt = duration_cast<duration<double>>(high_resolution_clock::now()-pt0).count();

        rut0 = high_resolution_clock::now();

        jGLParticles->update
        (
            jGL::Particles::UpdateInfo {true, false, false}
        );

        rut = duration_cast<duration<double>>(high_resolution_clock::now()-rut0).count();

        rdt0 = high_resolution_clock::now();

        jGLParticles->draw(camera.getVP());

        rdt = duration_cast<duration<double>>(high_resolution_clock::now()-rdt0).count();

        delta = 0.0;
        for (int n = 0; n < 60; n++)
        {
            delta += deltas[n];
        }
        delta /= 60.0;
        
        std::stringstream debugText;

        double mouseX, mouseY;
        display.mousePosition(mouseX,mouseY);

        debugText << "Delta: " << fixedLengthNumber(delta,6)
                  << " ( FPS: " << fixedLengthNumber(1.0/delta,4) 
                  << ")\n"
                  << "Physics/Render update/ Render draw time: \n" 
                  << "   " << fixedLengthNumber(pt, 6) << ", " << fixedLengthNumber(rut, 6) << ", " << fixedLengthNumber(rdt, 6) << "\n"
                  << "Mouse (" << fixedLengthNumber(mouseX,4) 
                  << "," 
                  << fixedLengthNumber(mouseY,4) 
                  << ")\n";

        jGLInstance->text(
            debugText.str(),
            glm::vec2(64.0f, resY-64.0f),
            0.5f,
            glm::vec4(0.0f,0.0f,0.0f,1.0f)
        );

        if (frameId == 30)
        {
            if (log.size() > 0)
            {
                std::cout << log << "\n";
            }
        }

        display.loop();

        tock = high_resolution_clock::now();

        deltas[frameId] = duration_cast<duration<double>>(tock-tic).count();
        frameId = (frameId+1) % 60;
    }

    return 0;
}