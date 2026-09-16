#include "CinemaController.hpp"

#include <engine/graphics/Camera.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <glm/gtc/matrix_transform.hpp>

void CinemaController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();

    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    // Ucitavamo modele
    m_room = resources->model("cinema");
    m_sofa = resources->model("sofa");
    m_platno = resources->model("platno");
    m_table = resources->model("table");
    m_projector = resources->model("projector");
    m_lamp = resources->model("lamp");

    // Ucitavamo shadere
    m_material_shader = resources->shader("material");
    m_texture_shader = resources->shader("texture");

    // Kamera
    graphics->camera()->Position = glm::vec3(0.0f, 2.0f, 15.0f);
    graphics->camera()->Yaw = -90.0f;
    graphics->camera()->Pitch = 0.0f;
    graphics->camera()->rotate_camera(0.0f, 0.0f);
}

bool CinemaController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    // ESC - izlaz iz programa
    if (platform->key(engine::platform::KEY_ESCAPE).state() ==
        engine::platform::Key::State::JustPressed) { return false; }

    // 1 - smanji directional light
    if (platform->key(engine::platform::KEY_1).state() ==
        engine::platform::Key::State::JustPressed) {
        m_directional_intensity -= 0.1f;

        if (m_directional_intensity < 0.0f) { m_directional_intensity = 0.0f; }
    }

    // 2 - pojacaj directional light
    if (platform->key(engine::platform::KEY_2).state() ==
        engine::platform::Key::State::JustPressed) {
        m_directional_intensity += 0.1f;

        if (m_directional_intensity > 2.0f) { m_directional_intensity = 2.0f; }
    }

    // 3 - smanji point light
    if (platform->key(engine::platform::KEY_3).state() ==
        engine::platform::Key::State::JustPressed) {
        m_point_intensity -= 0.1f;

        if (m_point_intensity < 0.0f) { m_point_intensity = 0.0f; }
    }

    // 4 - pojacaj point light
    if (platform->key(engine::platform::KEY_4).state() ==
        engine::platform::Key::State::JustPressed) {
        m_point_intensity += 0.1f;

        if (m_point_intensity > 2.0f) { m_point_intensity = 2.0f; }
    }

    // P - pokreni cinema dogadjaj
    if (platform->key(engine::platform::KEY_P).state() ==
        engine::platform::Key::State::JustPressed) {
        m_cinema_event_started = true;
        m_lights_dimmed = false;
        m_screen_brightened = false;

        m_event_start_time = platform->frame_time().current;
    }

    if (m_cinema_event_started) {
        float elapsed = platform->frame_time().current - m_event_start_time;

        // Posle 2 sekunde prigusi svetla
        if (elapsed >= 2.0f && !m_lights_dimmed) {
            m_directional_intensity = 0.2f;
            m_point_intensity = 0.2f;
            m_lights_dimmed = true;
        }

        // Posle ukupno 4 sekunde osvetli platno
        if (elapsed >= 4.0f && !m_screen_brightened) {
            m_screen_brightness = 2.0f;
            m_screen_brightened = true;
            m_cinema_event_started = false;
        }
    }

    return true;
}

void CinemaController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void CinemaController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    /*
     * TEXTURE SHADER
     */

    m_texture_shader->use();

    m_texture_shader->set_mat4("projection", graphics->projection_matrix());
    m_texture_shader->set_mat4("view", graphics->camera()->view_matrix());
    m_texture_shader->set_vec3("viewPosition", graphics->camera()->Position);

    // Directional light
    m_texture_shader->set_vec3(
            "directionalLight.direction",
            glm::vec3(-0.2f, -1.0f, -0.3f)
            );

    m_texture_shader->set_vec3(
            "directionalLight.ambient",
            glm::vec3(0.10f) * m_directional_intensity
            );

    m_texture_shader->set_vec3(
            "directionalLight.diffuse",
            glm::vec3(0.45f, 0.45f, 0.50f) * m_directional_intensity
            );

    m_texture_shader->set_vec3(
            "directionalLight.specular",
            glm::vec3(0.50f) * m_directional_intensity
            );

    // Point light
    m_texture_shader->set_vec3(
            "pointLight.position",
            glm::vec3(-3.0f, 3.0f, 2.5f)
            );

    m_texture_shader->set_vec3(
            "pointLight.ambient",
            glm::vec3(0.15f, 0.12f, 0.08f) * m_point_intensity
            );

    m_texture_shader->set_vec3(
            "pointLight.diffuse",
            glm::vec3(1.0f, 0.80f, 0.55f) * m_point_intensity
            );

    m_texture_shader->set_vec3(
            "pointLight.specular",
            glm::vec3(1.0f, 0.80f, 0.55f) * m_point_intensity
            );

    m_texture_shader->set_float("pointLight.constant", 1.0f);
    m_texture_shader->set_float("pointLight.linear", 0.09f);
    m_texture_shader->set_float("pointLight.quadratic", 0.032f);

    m_texture_shader->set_vec3("material.specular", glm::vec3(0.5f));
    m_texture_shader->set_float("material.shininess", 32.0f);

    // Prostorija
    glm::mat4 room_model = glm::mat4(1.0f);

    m_texture_shader->set_mat4("model", room_model);
    m_room->draw(m_texture_shader);

    // Sofa
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

    m_texture_shader->set_mat4("model", sofa_model);
    m_texture_shader->set_vec3("material.specular", glm::vec3(0.25f));
    m_texture_shader->set_float("material.shininess", 16.0f);
    m_sofa->draw(m_texture_shader);

    // Sto
    glm::mat4 table_model = glm::mat4(1.0f);

    table_model = glm::translate(
            table_model,
            glm::vec3(0.0f, 0.0f, 0.0f)
            );

    table_model = glm::scale(
            table_model,
            glm::vec3(0.7f)
            );

    m_texture_shader->set_mat4("model", table_model);
    m_texture_shader->set_vec3("material.specular", glm::vec3(0.8f));
    m_texture_shader->set_float("material.shininess", 64.0f);
    m_table->draw(m_texture_shader);

    // Lampa
    glm::mat4 lamp_model = glm::mat4(1.0f);

    lamp_model = glm::translate(
            lamp_model,
            glm::vec3(-3.0f, 0.0f, 2.5f)
            );

    lamp_model = glm::scale(
            lamp_model,
            glm::vec3(1.3f)
            );

    m_texture_shader->set_mat4("model", lamp_model);
    m_texture_shader->set_vec3("material.specular", glm::vec3(0.6f));
    m_texture_shader->set_float("material.shininess", 32.0f);
    m_lamp->draw(m_texture_shader);

    /*
     * MATERIAL SHADER
     */

    m_material_shader->use();

    m_material_shader->set_mat4("projection", graphics->projection_matrix());
    m_material_shader->set_mat4("view", graphics->camera()->view_matrix());
    m_material_shader->set_vec3("viewPosition", graphics->camera()->Position);

    // Directional light
    m_material_shader->set_vec3(
            "directionalLight.direction",
            glm::vec3(-0.2f, -1.0f, -0.3f)
            );

    m_material_shader->set_vec3(
            "directionalLight.ambient",
            glm::vec3(0.10f) * m_directional_intensity
            );

    m_material_shader->set_vec3(
            "directionalLight.diffuse",
            glm::vec3(0.45f, 0.45f, 0.50f) * m_directional_intensity
            );

    m_material_shader->set_vec3(
            "directionalLight.specular",
            glm::vec3(0.50f) * m_directional_intensity
            );

    // Point light
    m_material_shader->set_vec3(
            "pointLight.position",
            glm::vec3(-3.0f, 3.0f, 2.5f)
            );

    m_material_shader->set_vec3(
            "pointLight.ambient",
            glm::vec3(0.15f, 0.12f, 0.08f) * m_point_intensity
            );

    m_material_shader->set_vec3(
            "pointLight.diffuse",
            glm::vec3(1.0f, 0.80f, 0.55f) * m_point_intensity
            );

    m_material_shader->set_vec3(
            "pointLight.specular",
            glm::vec3(1.0f, 0.80f, 0.55f) * m_point_intensity
            );

    m_material_shader->set_float("pointLight.constant", 1.0f);
    m_material_shader->set_float("pointLight.linear", 0.09f);
    m_material_shader->set_float("pointLight.quadratic", 0.032f);

    // Platno
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

    m_material_shader->set_mat4("model", platno_model);

    m_material_shader->set_vec3(
            "material.ambient",
            glm::vec3(1.0f) * m_screen_brightness
            );

    m_material_shader->set_vec3(
            "material.diffuse",
            glm::vec3(1.0f) * m_screen_brightness
            );

    m_material_shader->set_vec3(
            "material.specular",
            glm::vec3(0.5f)
            );

    m_material_shader->set_float("material.shininess", 32.0f);

    m_platno->draw(m_material_shader);

    // Projektor
    glm::mat4 projector_model = glm::mat4(1.0f);

    projector_model = glm::translate(
            projector_model,
            glm::vec3(-0.8f, 3.0f, -4.5f)
            );

    m_material_shader->set_mat4("model", projector_model);

    m_material_shader->set_vec3(
            "material.ambient",
            glm::vec3(0.65f)
            );

    m_material_shader->set_vec3(
            "material.diffuse",
            glm::vec3(0.65f)
            );

    m_material_shader->set_vec3(
            "material.specular",
            glm::vec3(0.8f)
            );

    m_material_shader->set_float("material.shininess", 64.0f);

    m_projector->draw(m_material_shader);
}

void CinemaController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    platform->swap_buffers();
}