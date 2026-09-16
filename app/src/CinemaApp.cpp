#include "CinemaApp.hpp"
#include "CinemaController.hpp"

void CinemaApp::app_setup() {
    auto cinema_controller = register_controller<CinemaController>();

    cinema_controller->after(
            engine::core::Controller::get<engine::core::EngineControllersEnd>());
}