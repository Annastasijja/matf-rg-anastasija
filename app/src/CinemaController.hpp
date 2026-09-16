#pragma once

#include <engine/core/Controller.hpp>
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>

class CinemaController final : public engine::core::Controller {
public:
    void initialize() override;

    bool loop() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

private:
    engine::resources::Model *m_room{};
    engine::resources::Model *m_sofa{};
    engine::resources::Model *m_platno{};
    engine::resources::Model *m_table{};
    engine::resources::Model *m_projector{};
    engine::resources::Model *m_lamp{};
    engine::resources::Shader *m_material_shader{};
    engine::resources::Shader *m_texture_shader{};

    float m_directional_intensity = 1.0f;
    float m_point_intensity = 1.0f;

    bool m_cinema_event_started = false;
    bool m_lights_dimmed = false;
    bool m_screen_brightened = false;

    float m_event_start_time = 0.0f;

    float m_screen_brightness = 1.0f;
};
