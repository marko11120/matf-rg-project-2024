//
// Created by marko on 2/2/25.
//

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include "../include/MainController.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <iostream>

#include "glm/gtc/type_ptr.hpp"

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    void on_mouse_move(engine::platform::MousePosition position) override;
    void on_key(engine::platform::Key key) override;
    bool first_flick = true;
    bool cursor_visibility = false;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    if(first_flick) {
        first_flick = false;
        return;
    }
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    if(camera->get_cursor_status())
        return;

    camera->rotate_camera(position.dx, position.dy);
}

void MainPlatformEventObserver::on_key(engine::platform::Key key) {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();

    if(platform->key(engine::platform::KeyId::KEY_F12).is_down()) {
        if(!cursor_visibility) {
            cursor_visibility = true;
            camera->set_cursor_status(cursor_visibility);
            camera->set_cursor_visible(cursor_visibility);
        }else {
            cursor_visibility = false;
            camera->set_cursor_status(cursor_visibility);
            camera->set_cursor_visible(cursor_visibility);
        }
    }
}

void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    auto observer = std::make_unique<MainPlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(std::move(observer));
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    camera->set_cursor_visible(false);
    camera->set_cursor_status(false);
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
    model_matrix = glm::translate(model_matrix, glm::vec3(15.f, 30.f, -55.f));
    model_matrix = glm::rotate(model_matrix, glm::radians(timeValue), glm::vec3(1.0f, 1.0f, 0.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(0.6f));
    shader->set_mat4("model", model_matrix);
    model->draw(shader);
}

void MainController::draw_space_station() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *model   = resources->model("space_station");
    engine::resources::Shader *shader = resources->shader("space_station");

    shader->use();
    shader->set_vec3("light.ambient", this->light.ambient);
    shader->set_vec3("light.diffuse", this->light.diffuse);
    shader->set_vec3("light.specular", this->light.specular);
    shader->set_vec3("light.position", glm::vec3(15.f, 30.f, -55.f));

    shader->set_vec3("light.direction", graphics->camera()->Front);
    shader->set_float("light.cut_off", cos(glm::radians(12.5f)));
    shader->set_float("light.outerCut_off", cos(glm::radians(17.5f)));

    shader->set_float("material.linearC", 0.003f);
    shader->set_float("material.quadraticC", 0.0001f);
    shader->set_float("material.shininess", 32.f);
    shader->set_vec3("cameraPos", graphics->camera()->Position);


    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, -7.f, -40.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(0.08f));
    shader->set_mat4("model", model_matrix);
    model->draw(shader);
}

void MainController::draw_space_craft() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *model   = resources->model("spacecraft2");
    engine::resources::Shader *shader = resources->shader("space_craft");

    shader->use();
    shader->set_vec3("light.ambient", this->light.ambient);
    shader->set_vec3("light.diffuse", this->light.diffuse);
    shader->set_vec3("light.specular", this->light.specular);
    shader->set_vec3("light.position", glm::vec3(15.f, 30.f, -55.f));


    shader->set_float("material.linearC", 0.003f);
    shader->set_float("material.quadraticC", 0.0001f);
    shader->set_float("material.shininess", 32.f);
    shader->set_vec3("cameraPos", graphics->camera()->Position);

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, -2.0f, -40.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(0.4f));
    shader->set_mat4("model", model_matrix);
    model->draw(shader);
}

void MainController::draw_gui() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera   = graphics->camera();
    graphics->begin_gui();
    // Draw Camera Info window
    {
        ImGui::Begin("Camera info");
        ImGui::SetNextWindowPos(ImVec2(500, 350));
        ImGui::SetNextWindowSize(ImVec2(710, 300));
        const auto &c = *camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::SliderFloat("Camera x", &camera->Position.x, 0.1f, 10.0f, "%.2f");
        ImGui::SliderFloat("Camera y", &camera->Position.y, 0.1f, 10.0f, "%.2f");
        ImGui::SliderFloat("Camera z", &camera->Position.z, 0.1f, 10.0f, "%.2f");


        ImGui::Text("Ambient light intensity: (%f, %f, %f)", this->light.ambient.x, this->light.ambient.y, this->light.ambient.z);
        ImGui::SliderFloat3("Ambient slider", glm::value_ptr(this->light.ambient), 0.1f, 1.f);
        ImGui::Text("Diffuse light intensity: (%f, %f, %f)", this->light.diffuse.x, this->light.diffuse.y, this->light.diffuse.z);
        ImGui::SliderFloat3("Diffuse slider", glm::value_ptr(this->light.diffuse), 0.1f, 1.f);
        ImGui::Text("Specular light intensity: (%f, %f, %f)", this->light.specular.x, this->light.specular.y, this->light.specular.z);
        ImGui::SliderFloat3("Specular slider", glm::value_ptr(this->light.specular), 0.1f, 1.f);
        ImGui::End();
    }
    graphics->end_gui();
}



void MainController::draw() {
    draw_moon();
    draw_space_station();
    draw_space_craft();
    draw_skybox();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera   = graphics->camera();
    if (camera->get_cursor_status()) {
        draw_gui();
    }

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
    if(camera->get_cursor_status())
        return;

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


