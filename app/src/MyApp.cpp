#include <engine/platform/PlatformController.hpp>

#include "../include/MyApp.hpp"

#include "../../engine/libs/spdlog/include/spdlog/spdlog.h"
#include "MainController.hpp"

void MyApp::app_setup() {
    spdlog::info("Hello, setup!");
    auto main_controller = register_controller<MainController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}