#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/graphics/Camera.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

class CinemaController final : public engine::core::Controller {
public:
    void initialize() override {

        engine::graphics::OpenGL::enable_depth_testing();

        auto resources =
                engine::core::Controller::get<engine::resources::ResourcesController>();

        auto graphics =
                engine::core::Controller::get<engine::graphics::GraphicsController>();

        // Ucitavamo prostoriju
        room = resources->model("cinema");
        sofa = resources->model("sofa");
        platno = resources->model("platno");
        table = resources->model("table");
        projector = resources->model("projector");
        lamp = resources->model("lamp");

        // Ucitavamo shader
        shader = resources->shader("basic");

        // Kamera
        graphics->camera()->Position = glm::vec3(0.0f, 2.0f, 15.0f);
        graphics->camera()->Yaw = -90.0f;
        graphics->camera()->Pitch = 0.0f;
        graphics->camera()->rotate_camera(0.0f, 0.0f);
    }

    bool loop() override {

        auto platform =
                engine::core::Controller::get<
                    engine::platform::PlatformController>();

        // ESC - izlaz iz programa
        if (platform->key(engine::platform::KEY_ESCAPE).state() ==
            engine::platform::Key::State::JustPressed) { return false; }

        // 1 - smanji directional light
        if (platform->key(engine::platform::KEY_1).state() ==
            engine::platform::Key::State::JustPressed) {
            directionalIntensity -= 0.1f;

            if (directionalIntensity < 0.0f) directionalIntensity = 0.0f;
        }

        // 2 - pojacaj directional light
        if (platform->key(engine::platform::KEY_2).state() ==
            engine::platform::Key::State::JustPressed) {
            directionalIntensity += 0.1f;

            if (directionalIntensity > 2.0f) directionalIntensity = 2.0f;
        }

        // 3 - smanji point light
        if (platform->key(engine::platform::KEY_3).state() ==
            engine::platform::Key::State::JustPressed) {
            pointIntensity -= 0.1f;

            if (pointIntensity < 0.0f) pointIntensity = 0.0f;
        }

        // 4 - pojacaj point light
        if (platform->key(engine::platform::KEY_4).state() ==
            engine::platform::Key::State::JustPressed) {
            pointIntensity += 0.1f;

            if (pointIntensity > 2.0f) pointIntensity = 2.0f;
        }

        // P - pokreni cinema dogadjaj
        if (platform->key(engine::platform::KEY_P).state() ==
            engine::platform::Key::State::JustPressed) {
            cinemaEventStarted = true;
            lightsDimmed = false;
            screenBrightened = false;

            eventStartTime = std::chrono::steady_clock::now();
        }

        if (cinemaEventStarted) {
            auto now = std::chrono::steady_clock::now();

            float elapsed =
                    std::chrono::duration<float>(now - eventStartTime).count();

            // Posle 2 sekunde prigusi svetla
            if (elapsed >= 2.0f && !lightsDimmed) {
                directionalIntensity = 0.2f;
                pointIntensity = 0.2f;

                lightsDimmed = true;
            }

            // Posle ukupno 4 sekunde osvetli platno
            if (elapsed >= 4.0f && !screenBrightened) {
                screenBrightness = 2.0f;

                screenBrightened = true;
                cinemaEventStarted = false;
            }
        }

        return true;
    }

    void begin_draw() override { engine::graphics::OpenGL::clear_buffers(); }

    void draw() override {

        auto graphics =
                engine::core::Controller::get<
                    engine::graphics::GraphicsController>();

        shader->use();

        // Directional light
        shader->set_vec3(
                "directionalDirection",
                glm::vec3(-0.2f, -1.0f, -0.3f)
                );


        shader->set_vec3(
                "directionalColor",
                glm::vec3(0.45f, 0.45f, 0.50f) * directionalIntensity
                );

        // Point light - lampa
        shader->set_vec3(
                "pointPosition",
                glm::vec3(-3.0f, 3.0f, 2.5f)
                );

        shader->set_vec3(
                "pointColor",
                glm::vec3(1.0f, 0.80f, 0.55f) * pointIntensity
                );
        // Projection matrica
        shader->set_mat4(
                "projection",
                graphics->projection_matrix()
                );

        // View matrica kamere
        shader->set_mat4(
                "view",
                graphics->camera()->view_matrix()
                );


        glm::mat4 model = glm::mat4(1.0f);

        shader->set_mat4("model", model);
        shader->set_vec3("objectColor", glm::vec3(0.4f, 0.4f, 0.4f));
        shader->set_bool("hasDiffuseTexture", true);
        room->draw(shader);
        shader->set_bool("hasDiffuseTexture", false);

        glm::mat4 sofa_model = glm::mat4(1.0f);

        sofa_model = glm::translate(
                sofa_model,
                glm::vec3(-3.0f, 0.0f, 0.0f)
                );

        sofa_model = glm::rotate(
                sofa_model,
                glm::radians(180.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
                );

        sofa_model = glm::scale(
                sofa_model,
                glm::vec3(0.05f)
                );

        shader->set_bool("hasDiffuseTexture", true);
        shader->set_vec3("objectColor", glm::vec3(0.12f, 0.08f, 0.06f));

        shader->set_mat4("model", sofa_model);
        sofa->draw(shader);
        shader->set_bool("hasDiffuseTexture", false);

        glm::mat4 platno_model = glm::mat4(1.0f);

        platno_model = glm::translate(
                platno_model,
                glm::vec3(5.0f, 2.2f, 5.0f)
                );

        platno_model = glm::rotate(
                platno_model,
                glm::radians(90.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
                );

        platno_model = glm::scale(
                platno_model,
                glm::vec3(0.0015f)
                );

        shader->set_mat4("model", platno_model);

        shader->set_vec3(
                "objectColor",
                glm::vec3(1.0f, 1.0f, 1.0f) * screenBrightness
                );

        platno->draw(shader);

        glm::mat4 table_model = glm::mat4(1.0f);

        table_model = glm::translate(
                table_model,
                glm::vec3(0.0f, 0.0f, 0.0f)
                );

        table_model = glm::scale(
                table_model,
                glm::vec3(0.7f)
                );

        shader->set_mat4("model", table_model);


        shader->set_bool("hasDiffuseTexture", true);

        shader->set_vec3(
                "objectColor",
                glm::vec3(0.25f, 0.15f, 0.08f)
                );

        table->draw(shader);


        shader->set_bool("hasDiffuseTexture", false);

        glm::mat4 projector_model = glm::mat4(1.0f);

        projector_model = glm::translate(
                projector_model,
                glm::vec3(-0.8f, 3.0f, -4.5f)
                );

        shader->set_mat4("model", projector_model);

        shader->set_vec3(
                "objectColor",
                glm::vec3(0.65f, 0.65f, 0.65f)
                );

        projector->draw(shader);

        glm::mat4 lamp_model = glm::mat4(1.0f);

        lamp_model = glm::translate(
                lamp_model,
                glm::vec3(-3.0f, 0.0f, 2.5f)
                );

        lamp_model = glm::scale(
                lamp_model,
                glm::vec3(1.3f)
                );

        shader->set_mat4("model", lamp_model);

        // Lampa koristi svoju teksturu
        shader->set_bool("hasDiffuseTexture", true);

        shader->set_vec3(
                "objectColor",
                glm::vec3(0.55f, 0.45f, 0.30f)
                );

        lamp->draw(shader);


        shader->set_bool("hasDiffuseTexture", false);
    }

    void end_draw() override {

        auto platform =
                engine::core::Controller::get<
                    engine::platform::PlatformController>();

        platform->swap_buffers();
    }

private:
    engine::resources::Model *room{};
    engine::resources::Model *sofa{};
    engine::resources::Model *platno{};
    engine::resources::Model *table{};
    engine::resources::Model *projector{};
    engine::resources::Model *lamp{};
    engine::resources::Shader *shader{};
    float directionalIntensity = 1.0f;
    float pointIntensity = 1.0f;

    bool cinemaEventStarted = false;
    bool lightsDimmed = false;
    bool screenBrightened = false;

    std::chrono::steady_clock::time_point eventStartTime;

    float screenBrightness = 1.0f;
};

class CinemaApp final : public engine::core::App {

public:
    void app_setup() override {

        auto cinema_controller =
                register_controller<CinemaController>();

        cinema_controller->after(
                engine::core::Controller::get<
                    engine::core::EngineControllersEnd>()
                );
    }
};

int main(int argc, char **argv) { return std::make_unique<CinemaApp>()->run(argc, argv); }