#include <main.h>

int main(int argv, char ** argc)
{

    jGL::Display display(resX, resY, "Sprite");

    glewInit();
    
    jGLInstance = std::move(std::make_unique<jGL::GL::OpenGLInstance>(display));

    jGL::OrthoCam camera(resX, resY, glm::vec2(0.0,0.0));

    camera.setPosition(0.0f, 0.0f);

    glm::vec2 res = camera.getResolution();
    glm::vec4 minWorld = camera.screenToWorld(0.0f, 0.0f);
    glm::vec4 maxWorld = camera.screenToWorld(res.x, res.y);

    Log log;

    high_resolution_clock::time_point tic, tock;
    double rdt = 0.0;

    jGLInstance->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));

    std::shared_ptr<jGL::Texture> heart = jGLInstance->createTexture
    (
        "resource/texture/HEART.png",
        jGL::Texture::Type::RGBA
    );

    std::shared_ptr<jGL::Texture> Pi = jGLInstance->createTexture
    (
        "resource/texture/Pi.png",
        jGL::Texture::Type::RGBA
    );

    std::shared_ptr<jGL::Texture> random = jGLInstance->createTexture
    (
        "resource/texture/random.png",
        jGL::Texture::Type::RGBA
    );

    double delta = 0.0;

    while (display.isOpen())
    {
        tic = high_resolution_clock::now();

        jGLInstance->clear();

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
                  << "Render draw time: \n" 
                  << "   " << fixedLengthNumber(rdt, 6) << "\n"
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