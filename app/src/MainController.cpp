//
// Created by marko on 2/2/25.
//

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include "../include/MainController.h"
#include <GLFW/glfw3.h>

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();

    camera->rotate_camera(position.dx, position.dy);
}


void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    auto observer = std::make_unique<MainPlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(std::move(observer));
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    camera->set_cursor_visible(false);
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if(platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down())
        return false;
    return true;
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("stars_skybox");
    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::draw_moon() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *model   = resources->model("Moon");
    engine::resources::Shader *shader = resources->shader("moon");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model_matrix = glm::mat4(1.0f);
    float timeValue = 10 * glfwGetTime();
    model_matrix = glm::translate(model_matrix, glm::vec3(5.f, 4.f, -30.f));
    model_matrix = glm::rotate(model_matrix, glm::radians(timeValue), glm::vec3(1.0f, 1.0f, 0.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(0.3f));
    shader->set_mat4("model", model_matrix);
    model->draw(shader);
}

void MainController::draw() {
    draw_skybox();
    draw_moon();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}


void MainController::update() {
    update_camera();
}

void MainController::update_camera() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float delta = platform->dt();

    if(platform->key(engine::platform::KeyId::KEY_W).is_down()) {
        camera->move_camera(engine::graphics::Camera::FORWARD, delta);
    }
    if(platform->key(engine::platform::KeyId::KEY_A).is_down()) {
        camera->move_camera(engine::graphics::Camera::LEFT, delta);
    }
    if(platform->key(engine::platform::KeyId::KEY_S).is_down()) {
        camera->move_camera(engine::graphics::Camera::BACKWARD, delta);
    }
    if(platform->key(engine::platform::KeyId::KEY_D).is_down()) {
        camera->move_camera(engine::graphics::Camera::RIGHT, delta);
    }
}

