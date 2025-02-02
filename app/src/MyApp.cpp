#include "../include/MyApp.h"

#include "../../engine/libs/spdlog/include/spdlog/spdlog.h"
#include "MainController.h"

void MyApp::app_setup() {
    spdlog::info("Hello, setup!");
    auto main_controller = register_controller<MainController>();
}