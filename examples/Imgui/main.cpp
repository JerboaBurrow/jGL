#include "main.h"

int main(int argv, char ** argc)
{

    jGL::DesktopDisplay::Config conf;

    conf.VULKAN = false;

    #ifdef MACOS
    conf.COCOA_RETINA = true;
    #endif
    jGL::DesktopDisplay display(glm::ivec2(resX, resY), "Shape", conf);
    display.setFrameLimit(60);

    glewInit();

    glm::ivec2 res = display.frameBufferSize();
    resX = res.x;
    resY = res.y;

    jGLInstance = std::move(std::make_unique<jGL::GL::OpenGLInstance>(res));

    jGL::OrthoCam camera(resX, resY, glm::vec2(0.0,0.0));

    camera.setPosition(0.0f, 0.0f);

    jLog::Log log;

    high_resolution_clock::time_point tic, tock;
    double rdt = 0.0;

    jGLInstance->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));
    jGLInstance->setMSAA(1);

    std::vector<jGL::Shape> shapes;
    std::vector<jGL::Transform> trans;
    std::vector<glm::vec4> cols;

    RNG rng;
    uint64_t n = 100000;

    std::shared_ptr<jGL::ShapeRenderer> circles = jGLInstance->createShapeRenderer
    (
        n
    );

    shapes.reserve(n);
    trans.reserve(n);
    cols.reserve(n);
    for (unsigned i = 0; i < n; i++)
    {
        trans.push_back(jGL::Transform(rng.nextFloat(), rng.nextFloat(), 0.0, 0.001f));
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

    std::shared_ptr<jGL::Shader> shader = std::make_shared<jGL::GL::glShader>
    (
        jGL::GL::glShapeRenderer::shapeVertexShader,
        jGL::GL::glShapeRenderer::ellipseFragmentShader
    );

    shader->use();

    double delta = 0.0;
    double dt = 1.0/600.0;
    jGL::ShapeRenderer::UpdateInfo uinfo;

    display.initImgui();

    while (display.isOpen())
    {
        tic = high_resolution_clock::now();

        jGLInstance->beginFrame();

            jGLInstance->clear();

            for (unsigned i = 0; i <shapes.size(); i++)
            {
                auto & tr = trans[i];
                tr.x = tr.x+dt*(rng.nextFloat()-0.5);
                tr.y = tr.y+dt*(rng.nextFloat()-0.5);
                tr.theta = tr.theta;
                tr.scaleX = tr.scaleX;
            }

            circles->draw(shader, uinfo);

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

        ImGui::Begin("Hello, world!");

        ImGui::Text("This is some useful text.");

        if (ImGui::Button("Button"))
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
        uinfo.colour = false;
        uinfo.scale = false;

    }

    jGLInstance->finish();

    return 0;
}