#include "main.h"

int main(int argv, char ** argc)
{

    jGL::DesktopDisplay::Config conf;

    conf.VULKAN = false;
    conf.COCOA_RETINA = false;

    jGL::DesktopDisplay display(glm::ivec2(resX, resY), "Shape", conf);

    glewInit();

    jGLInstance = std::move(std::make_unique<jGL::GL::OpenGLInstance>(display.getRes()));

    jGL::OrthoCam camera(resX, resY, glm::vec2(0.0,0.0));

    camera.setPosition(0.0f, 0.0f);

    jLog::Log log;

    high_resolution_clock::time_point tic, tock;
    double rdt = 0.0;

    jGLInstance->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));
    jGLInstance->setMSAA(1);

    std::shared_ptr<jGL::ShapeRenderer> circles = jGLInstance->createShapeRenderer
    (
        32
    );

    std::vector<jGL::Shape> shapes;

    RNG rng;

    std::vector<jGL::Transform> trans;
    std::vector<glm::vec4> cols;

    trans.reserve(64);
    shapes.reserve(64);
    cols.reserve(64);

    for (unsigned i = 0; i < 64; i++)
    {
        trans.push_back(jGL::Transform(rng.nextFloat(), rng.nextFloat(), 0.0, 0.1f));
        cols.push_back(glm::vec4(rng.nextFloat(), rng.nextFloat(), rng.nextFloat(), 1.0));

        shapes.push_back
        (
            {
                &trans[i],
                &cols[i]
            }
        );

        circles->add(shapes[i], std::to_string(i));
    }

    circles->setProjection(camera.getVP());

    std::shared_ptr<jGL::Shader> shader = std::make_shared<jGL::GL::glShader>(vertexShader, fragmentShader);

    shader->use();

    double delta = 0.0;
    double dt = 1.0/600.0;

    display.initImgui();

    while (display.isOpen())
    {
        tic = high_resolution_clock::now();

        jGLInstance->beginFrame();

            jGLInstance->clear();

            for (unsigned i = 0; i <shapes.size(); i++)
            {
                auto tr = circles->getTransform(std::to_string(i));
                trans[i] = jGL::Transform
                (
                    tr->x+dt*(rng.nextFloat()-0.5),
                    tr->y+dt*(rng.nextFloat()-0.5),
                    tr->theta,
                    tr->scaleX
                );
            }

            circles->draw(shader);

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

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        display.loop();

        tock = high_resolution_clock::now();

        deltas[frameId] = duration_cast<duration<double>>(tock-tic).count();
        frameId = (frameId+1) % 60;

    }

    jGLInstance->finish();

    return 0;
}