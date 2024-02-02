#include <main.h>

int main(int argv, char ** argc)
{

    jGL::DesktopDisplay::Config conf;

    conf.VULKAN = false;
    conf.COCOA_RETINA = false;

    jGL::DesktopDisplay display(glm::ivec2(resX, resY), "Sprite", conf);

    glewInit();
    
    jGLInstance = std::move(std::make_unique<jGL::GL::OpenGLInstance>(display.getRes()));

    jGL::OrthoCam camera(resX, resY, glm::vec2(0.0,0.0));

    camera.setPosition(0.0f, 0.0f);
    
    jLog::Log log;

    high_resolution_clock::time_point tic, tock;
    double rdt = 0.0;

    jGLInstance->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));
    jGLInstance->setMSAA(1);

    std::shared_ptr<jGL::Texture> heart = jGLInstance->createTexture
    (
        "resource/texture/HEART.png",
        jGL::Texture::Type::RGBA
    );

    std::shared_ptr<jGL::Texture> dice = jGLInstance->createTexture
    (
        "resource/texture/random.png",
        jGL::Texture::Type::RGBA
    );

    std::shared_ptr<jGL::Texture> Pi = jGLInstance->createTexture
    (
        "resource/texture/Pi.png",
        jGL::Texture::Type::RGBA
    );

    std::shared_ptr<jGL::SpriteRenderer> sprites = jGLInstance->createSpriteRenderer
    (
        2
    );

    sprites->setProjection(camera.getVP());

    sprites->add
    (
        {
            jGL::Transform(0.1f, 0.1f, 0.0f, 0.1f),
            jGL::TextureOffset(0.0f, 0.0f),
            Pi
        },
        "sPi"
    );

    sprites->add
    (
        {
            jGL::Transform(0.5f, 0.5f, 0.0f, 0.1f),
            jGL::TextureOffset(0.0f, 0.0f),
            heart
        },
        "sHeart"
    );

    sprites->add
    (
        {
            jGL::Transform(0.6f, 0.2f, 0.5f, 0.15f),
            jGL::TextureOffset(0.0f, 0.0f),
            dice
        },
        "sDice"
    );

        sprites->add
    (
        {
            jGL::Transform(0.5f, 0.1f, 0.0f, 0.1f),
            jGL::TextureOffset(0.0f, 0.0f),
            Pi
        },
        "lowest"
    );

    sprites->add
    (
        {
            jGL::Transform(0.55f, 0.15f, 0.0f, 0.1f),
            jGL::TextureOffset(0.0f, 0.0f),
            heart
        },
        "middle",
        1000
    );

    sprites->add
    (
        {
            jGL::Transform(0.6f, 0.2f, 0.0f, 0.1f),
            jGL::TextureOffset(0.0f, 0.0f),
            dice
        },
        "highest",
        100000
    );

    double delta = 0.0;

    float theta = 0.0f;
    float scale = 0.0f;

    while (display.isOpen())
    {
        tic = high_resolution_clock::now();

        jGLInstance->beginFrame();

            jGLInstance->clear();

            theta += 1.0/60.0 * 0.1;
            scale = 0.1*std::abs(std::sin(theta))+0.05;

            sprites->getSprite("sHeart").update(jGL::Transform(0.5f, 0.5f, theta, 0.1f));
            sprites->getSprite("sPi").update(jGL::Transform(0.2f, 0.2f, theta, scale));

            sprites->draw();

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

        jGLInstance->endFrame();

        display.loop();

        tock = high_resolution_clock::now();

        deltas[frameId] = duration_cast<duration<double>>(tock-tic).count();
        frameId = (frameId+1) % 60;
            
    }

    jGLInstance->finish();

    return 0;
}