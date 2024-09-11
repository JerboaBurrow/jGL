#include <main.h>

int main(int argv, char ** argc)
{

    jGL::DesktopDisplay::Config conf;

    conf.VULKAN = false;
    #ifdef MACOS
    conf.COCOA_RETINA = true;
    #endif

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

    std::shared_ptr<jGL::Texture> jerboa = jGLInstance->createTexture
    (
        std::vector(LOGO32.begin(), LOGO32.end()),
        jGL::Texture::Type::RGBA
    );

    int n = 500000;

    std::shared_ptr<jGL::SpriteRenderer> sprites = jGLInstance->createSpriteRenderer
    (
        n
    );

    float scale = camera.screenToWorld(8.0f, 0.0f).x;

    std::vector<jGL::TextureRegion> animationFrames
    {
        jGL::TextureRegion(0, 0, 16, 16),
        jGL::TextureRegion(16, 0, 16, 16),
        jGL::TextureRegion(0, 16, 16, 16),
        jGL::TextureRegion(16, 16, 16, 16)
    };

    glm::vec4 colour(1.0);

    std::vector<jGL::Transform> trans;
    trans.reserve(n);
    RNG rng;
    for (int i = 0; i < n; i++)
    {
        trans.push_back(jGL::Transform(rng.nextFloat(), rng.nextFloat(), 0.0, scale));
    }

    for (int i = 0; i < n; i++)
    {
        sprites->add
        (
            {&trans[i], &animationFrames[i % animationFrames.size()], jerboa.get(), &colour},
            std::to_string(i)
        );
    }

    sprites->setProjection(camera.getVP());

    double delta = 0.0;

    float dt = 1.0/600.0;

    while (display.isOpen())
    {
        tic = high_resolution_clock::now();

        jGLInstance->beginFrame();

            jGLInstance->clear();

            for (auto & tr : trans)
            {
                tr.x = tr.x+dt*(rng.nextFloat()-0.5);
                tr.y = tr.y+dt*(rng.nextFloat()-0.5);
                tr.theta = tr.theta;
                tr.scaleX = tr.scaleX;
            }

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
                glm::vec2(resX*0.5f, resY-64.0f),
                0.5f,
                glm::vec4(0.0f,0.0f,0.0f,1.0f),
                glm::bvec2(true,true)
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