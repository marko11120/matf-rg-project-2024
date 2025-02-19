#include "MyApp.hpp"
#include "MainController.hpp"
#include "spdlog/spdlog.h"

void MyApp::app_setup() {
    spdlog::info("Hello, setup!");
    auto main_controller = register_controller<MainController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}