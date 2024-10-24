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

    std::shared_ptr<jGL::Texture> atlas = jGLInstance->createTexture
    (
        "resource/texture/atlas.png",
        jGL::Texture::Type::RGBA
    );

    std::shared_ptr<jGL::Texture> jerboa = jGLInstance->createTexture
    (
        std::vector(LOGO32.begin(), LOGO32.end()),
        jGL::Texture::Type::RGBA
    );

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

    std::shared_ptr<jGL::SpriteRenderer> sprites = jGLInstance->createSpriteRenderer
    (
        2
    );

    float scale32Pixels = camera.screenToWorld(32.0f, 0.0f).x;
    float ax = 0.25f;
    float ay = 0.75f;
    float as = scale32Pixels;

    std::map<std::string, jGL::Transform> trans =
    {
        {"sAtlas1", jGL::Transform(ax, ay, 0.0f, scale32Pixels*0.5f)},
        {"sAtlas2", jGL::Transform(ax+as, ay, 0.0f, scale32Pixels*0.5f)},
        {"sAtlas3", jGL::Transform(ax, ay+as, 0.0f, scale32Pixels*0.5f)},
        {"sAtlas4", jGL::Transform(ax+as, ay+as, 0.0f, scale32Pixels*0.5f)},
        {"sAtlasFull", jGL::Transform(ax, ay+as+scale32Pixels, 0.0f, scale32Pixels)},
        {"sAtlasAnimated", jGL::Transform(ax+as, ay-scale32Pixels, 0.0f, scale32Pixels*0.5f)},
        {"sJerboa", jGL::Transform(0.1f, 0.9f, 0.0f, 0.1f)},
        {"sPi", jGL::Transform(0.1f, 0.1f, 0.0f, 0.1f)},
        {"sHeart", jGL::Transform(0.5f, 0.5f, 0.0f, 0.1f)},
        {"sRandom", jGL::Transform(0.5f, 0.25f, 0.0f, 0.1f, 0.05f)},
        {"lowest", jGL::Transform(0.5f, 0.1f, 0.0f, 0.1f)},
        {"middle", jGL::Transform(0.55f, 0.15f, 0.0f, 0.1f)},
        {"highest", jGL::Transform(0.6f, 0.2f, 0.0f, 0.1f)}
    };

    std::vector<jGL::TextureRegion> animationFrames
    {
        jGL::TextureRegion(0, 0, 16, 16),
        jGL::TextureRegion(16, 0, 16, 16),
        jGL::TextureRegion(0, 16, 16, 16),
        jGL::TextureRegion(16, 16, 16, 16)
    };

    std::map<std::string, jGL::TextureRegion> reg =
    {
        {"sAtlas1", jGL::TextureRegion(0, 0, 16, 16)},
        {"sAtlas2", jGL::TextureRegion(16, 0, 16, 16)},
        {"sAtlas3", jGL::TextureRegion(0, 16, 16, 16)},
        {"sAtlas4", jGL::TextureRegion(16, 16, 16, 16)},
        {"sAtlasFull", animationFrames[0]},
        {"sAtlasAnimated", jGL::TextureRegion(0, 0, 16, 16)},
        {"sJerboa", jGL::TextureRegion()},
        {"sPi", jGL::TextureRegion()},
        {"sHeart", jGL::TextureRegion()},
        {"sRandom", jGL::TextureRegion()},
        {"lowest", jGL::TextureRegion()},
        {"middle", jGL::TextureRegion()},
        {"highest", jGL::TextureRegion()}
    };

    glm::vec4 colour(1.0);

    sprites->setProjection(camera.getVP());

    sprites->add
    (
        {
            &trans["sJerboa"],
            &reg["sJerboa"],
            jerboa.get(),
            &colour
        },
        "sJerboa"
    );

    sprites->add
    (
        {
            &trans["sAtlas1"],
            &reg["sAtlas1"],
            atlas.get(),
            &colour
        },
        "sAtlas1"
    );

    sprites->add
    (
        {
            &trans["sAtlas2"],
            &reg["sAtlas2"],
            atlas.get(),
            &colour
        },
        "sAtlas2"
    );

    sprites->add
    (
        {
            &trans["sAtlas3"],
            &reg["sAtlas3"],
            atlas.get(),
            &colour
        },
        "sAtlas3"
    );

    sprites->add
    (
        {
            &trans["sAtlas4"],
            &reg["sAtlas4"],
            atlas.get(),
            &colour
        },
        "sAtlas4"
    );

    sprites->add
    (
        {
            &trans["sAtlasFull"],
            &reg["sAtlasFull"],
            atlas.get(),
            &colour
        },
        "sAtlasFull"
    );

    uint8_t animationFrame = 0;

    sprites->add
    (
        {
            &trans["sAtlasAnimated"],
            &reg["sAtlasAnimated"],
            atlas.get(),
            &colour
        },
        "sAtlasAnimated"
    );


    sprites->add
    (
        {
            &trans["sPi"],
            &reg["sPi"],
            Pi.get(),
            &colour
        },
        "sPi"
    );

    sprites->add
    (
        {
            &trans["sHeart"],
            &reg["sHeart"],
            heart.get(),
            &colour
        },
        "sHeart"
    );

    sprites->add
    (
        {
            &trans["sRandom"],
            &reg["sRandom"],
            random.get(),
            &colour
        },
        "sRandom"
    );

        sprites->add
    (
        {
            &trans["lowest"],
            &reg["lowest"],
            Pi.get(),
            &colour
        },
        "lowest"
    );

    sprites->add
    (
        {
            &trans["middle"],
            &reg["middle"],
            heart.get(),
            &colour
        },
        "middle",
        1000
    );

    sprites->add
    (
        {
            &trans["highest"],
            &reg["highest"],
            jerboa.get(),
            &colour
        },
        "highest",
        100000
    );

    std::shared_ptr<jGL::ShapeRenderer> shapes = jGLInstance->createShapeRenderer
    (
        2
    );

    std::vector<jGL::Transform> shapeTrans =
    {
        jGL::Transform(0.0, 0.0, 0.0, 1.0),
        jGL::Transform(0.0, 0.0, 0.0, 1.0)
    };
    std::vector<glm::vec4> shapeCols =
    {
        glm::vec4(1.0, 0.0, 0.0, 0.3),
        glm::vec4(1.0, 0.0, 0.0, 0.3)
    };

    auto bbPi = jGL::Shape
    (
        &shapeTrans[0],
        &shapeCols[0]
    );

    auto bbHeart = jGL::Shape
    (
        &shapeTrans[1],
        &shapeCols[1]
    );

    shapes->add(bbPi, "pi");
    shapes->add(bbHeart, "heart");
    shapes->setProjection(camera.getVP());

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

            trans["sHeart"] = jGL::Transform(0.5f, 0.5f, theta, 0.1f);
            trans["sPi"] = jGL::Transform(0.2f, 0.2f, theta, scale);

            reg["sAtlasAnimated"] = animationFrames[animationFrame];
            if (frameId % 15 == 0)
            {
                animationFrame = (animationFrame+1)%animationFrames.size();
            }

            shapeTrans[0] = sprites->getSprite("sPi").getWorldBoundingBox().toTransform();
            shapeTrans[1] =  sprites->getSprite("sHeart").getWorldBoundingBox().toTransform();

            sprites->draw();
            shapes->draw();

            if (frameId == 1)
            {
                sprites->remove("sJerboa");
                shapes->remove("heart");
            }
            else if (frameId == 30)
            {
                sprites->add
                (
                    {
                        &trans["sJerboa"],
                        &reg["sJerboa"],
                        jerboa.get(),
                        &colour
                    },
                    "sJerboa"
                );
                shapes->add(bbHeart, "heart");
            }

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
                    << ")\n"
                    << "Mouse in Pi's BB? " << sprites->getSprite("sPi").getScreenBoundingBox(camera).in(mouseX, mouseY);

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