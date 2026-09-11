#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/graphics/Camera.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

        return true;
    }

    void begin_draw() override { engine::graphics::OpenGL::clear_buffers(); }

    void draw() override {

        auto graphics =
                engine::core::Controller::get<
                    engine::graphics::GraphicsController>();

        shader->use();

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

        // Prostorija je vec napravljena u OBJ fajlu,
        // zato nam je model matrica identitet.
        glm::mat4 model = glm::mat4(1.0f);

        shader->set_mat4("model", model);

        room->draw(shader);

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

        shader->set_mat4("model", sofa_model);
        sofa->draw(shader);

        glm::mat4 platno_model = glm::mat4(1.0f);

        platno_model = glm::translate(
                platno_model,
                glm::vec3(5.0f, 2.2f, 0.0f)
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
        table->draw(shader);

        glm::mat4 projector_model = glm::mat4(1.0f);

        projector_model = glm::translate(
                projector_model,
                glm::vec3(-0.8f, 3.0f, -4.5f)
                );

        shader->set_mat4("model", projector_model);
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
        lamp->draw(shader);
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