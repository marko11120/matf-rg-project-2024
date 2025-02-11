
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <MainController.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include "spdlog/spdlog.h"
#include <Framebuffer.hpp>

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
private:
    void on_mouse_move(engine::platform::MousePosition position) override;
    void on_key(engine::platform::Key key) override;
    bool m_first_flick       = true;
    bool m_cursor_visibility = false;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    if (m_first_flick) {
        m_first_flick = false;
        return;
    }
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera   = graphics->camera();
    if (platform->get_cursor_status())
        return;

    camera->rotate_camera(position.dx, position.dy);
}

void MainPlatformEventObserver::on_key(engine::platform::Key key) {
    auto platform       = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics       = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto main_controller = platform->get<MainController>();

    if (platform->key(engine::platform::KeyId::KEY_F12).is_down()) {
        m_cursor_visibility = !m_cursor_visibility;
        platform->set_cursor_visible(m_cursor_visibility);
    } else if (platform->key(engine::platform::KeyId::KEY_E).is_down()) {
        // 0 turning on
        // 1 shutting down
        main_controller->moon_event = (main_controller->moon_event + 1) % 2;
    } else if (platform->key(engine::platform::KeyId::KEY_UP).is_down()) {
        main_controller->spacecraft_pos.y = glm::min(main_controller->spacecraft_pos.y + platform->dt() * 3.f, 30.f);
    } else if (platform->key(engine::platform::KeyId::KEY_DOWN).is_down()) {
        main_controller->spacecraft_pos.y = glm::max(main_controller->spacecraft_pos.y - platform->dt() * 3.f, -2.f);
    } else if (platform->key(engine::platform::KeyId::KEY_LEFT).is_down()) {
        main_controller->spacecraft_rotation += platform->dt() * 1.f;
    } else if (platform->key(engine::platform::KeyId::KEY_RIGHT).is_down()) {
        main_controller->spacecraft_rotation -= platform->dt() * 1.f;
    }
}

void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    auto observer = std::make_unique<MainPlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(
            std::move(observer));
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->set_cursor_visible(false);

    m_framebuffer = new Framebuffer();
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down())
        return false;
    return true;
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}



void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox    = resources->skybox("stars_skybox");
    auto shader    = resources->shader("skybox");
    auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::draw_moon() {
    auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics                     = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *model   = resources->model("Moon");
    engine::resources::Shader *shader = resources->shader("moon");
    auto platform                     = engine::core::Controller::get<engine::platform::PlatformController>();
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    float timeValue = platform->frame_time().current;
    // float pom = (cos(timeValue) + 1)/2.f; // fade function
    if (moon_event == 1) {
        m_light.intensity = glm::vec3(0.2);
    } else if (moon_event == 0) {
        m_light.intensity = glm::vec3(1.f);
    }

    shader->set_vec3("light_intensity", m_light.intensity);
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix           = glm::translate(model_matrix, m_light.position);
    model_matrix           = glm::rotate(model_matrix, glm::radians(timeValue), glm::vec3(1.0f, 1.0f, 0.0f));
    model_matrix           = glm::scale(model_matrix, glm::vec3(0.6f));
    shader->set_mat4("model", model_matrix);
    model->draw(shader);
}

void MainController::draw_space_station() {
    auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics                     = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *model   = resources->model("space_station");
    engine::resources::Shader *shader = resources->shader("space_station");

    shader->use();
    shader->set_vec3("light.ambient", m_light.ambient);
    shader->set_vec3("light.diffuse", m_light.diffuse);
    shader->set_vec3("light.specular", m_light.specular);
    shader->set_vec3("light.position", m_light.position);
    shader->set_vec3("light.intensity", m_light.intensity);

    shader->set_vec3("spotLight.direction", graphics->camera()->Front);
    shader->set_float("spotLight.cut_off", m_spot_light.cut_off);
    shader->set_float("spotLight.outerCut_off", m_spot_light.outterCut_off);
    shader->set_vec3("spotLight.diffuse", m_spot_light.diffuse);
    shader->set_vec3("spotLight.specular", m_spot_light.specular);

    shader->set_float("material.linearC", 0.003f);
    shader->set_float("material.quadraticC", 0.0001f);
    shader->set_float("material.shininess", 32.f);

    shader->set_vec3("cameraPos", graphics->camera()->Position);

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix           = glm::translate(model_matrix, glm::vec3(0.0f, -7.f, -30.0f));
    model_matrix           = glm::scale(model_matrix, glm::vec3(0.08f));
    shader->set_mat4("model", model_matrix);
    model->draw(shader);
}

void MainController::draw_space_craft() {
    auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics                     = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *model   = resources->model("spacecraft2");
    engine::resources::Shader *shader = resources->shader("space_craft");

    shader->use();
    shader->set_vec3("light.ambient", m_light.ambient);
    shader->set_vec3("light.diffuse", m_light.diffuse);
    shader->set_vec3("light.specular", m_light.specular);
    shader->set_vec3("light.position", m_light.position);
    shader->set_vec3("light.intensity", m_light.intensity);

    shader->set_float("material.linearC", 0.003f);
    shader->set_float("material.quadraticC", 0.0001f);
    shader->set_float("material.shininess", 32.f);

    shader->set_vec3("spotLight.direction", graphics->camera()->Front);
    shader->set_float("spotLight.cut_off", m_spot_light.cut_off);
    shader->set_float("spotLight.outerCut_off", m_spot_light.outterCut_off);
    shader->set_vec3("spotLight.diffuse", m_spot_light.diffuse);
    shader->set_vec3("spotLight.specular", m_spot_light.specular);

    shader->set_vec3("cameraPos", graphics->camera()->Position);

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix           = glm::translate(model_matrix, spacecraft_pos);
    model_matrix           = glm::rotate(model_matrix, spacecraft_rotation, glm::vec3(0.f, 1.f, 0.f));
    model_matrix           = glm::scale(model_matrix, glm::vec3(0.4f));
    shader->set_mat4("model", model_matrix);
    model->draw(shader);
}

void MainController::draw_gui() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera   = graphics->camera();
    graphics->begin_gui();
    {
        ImGui::Begin("Camera info");
        ImGui::SetNextWindowPos(ImVec2(500, 350));
        ImGui::SetNextWindowSize(ImVec2(710, 300));
        const auto &c = *camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::SliderFloat("Camera x", &camera->Position.x, -20.f, 20.0f, "%.2f");
        ImGui::SliderFloat("Camera y", &camera->Position.y, -20.f, 20.0f, "%.2f");
        ImGui::SliderFloat("Camera z", &camera->Position.z, -20.f, 20.0f, "%.2f");

        ImGui::Text("Point light sliders:");
        ImGui::Text("Point light ambient intensity: (%f, %f, %f)", m_light.ambient.x, m_light.ambient.y,
                    m_light.ambient.z);
        ImGui::SliderFloat3("Point light ambient slider", glm::value_ptr(m_light.ambient), 0.1f, 1.f);
        ImGui::Text("Point diffuse light intensity: (%f, %f, %f)", m_light.diffuse.x, m_light.diffuse.y,
                    m_light.diffuse.z);
        ImGui::SliderFloat3("Point diffuse slider", glm::value_ptr(m_light.diffuse), 0.1f, 1.f);
        ImGui::Text("Point light specular intensity: (%f, %f, %f)", m_light.specular.x, m_light.specular.y,
                    m_light.specular.z);
        ImGui::SliderFloat3("Point specular slider", glm::value_ptr(m_light.specular), 0.1f, 1.f);

        ImGui::Text("Spot light sliders:");
        ImGui::Text("Spot light diffuse intensity: (%f, %f, %f)", m_spot_light.diffuse.x, m_spot_light.diffuse.y,
                    m_spot_light.diffuse.z);
        ImGui::SliderFloat3("Spot light diffuse slider", glm::value_ptr(m_spot_light.diffuse), 0.1f, 1.f);
        ImGui::Text("Spot light diffuse intensity: (%f, %f, %f)", m_spot_light.specular.x,
                    m_spot_light.specular.y, m_spot_light.specular.z);
        ImGui::SliderFloat3("Spot light specular slider", glm::value_ptr(m_spot_light.specular), 0.1f, 1.f);
        ImGui::Text("Cut off angle: (%f)", m_spot_light.cut_off);
        ImGui::SliderFloat("Cut off slider", &m_spot_light.cut_off, -1.0f, 1.f);
        ImGui::Text("Outer cut off angle: (%f)", m_spot_light.outterCut_off);
        ImGui::SliderFloat("Outer cut off slider", &m_spot_light.outterCut_off, -1.0f, 1.f);
        ImGui::End();
    }
    graphics->end_gui();
}

void MainController::draw() {
    engine::graphics::OpenGL::bind_framebuffer(m_framebuffer->get_framebuffer());
    engine::graphics::OpenGL::enable_depth_testing();
    engine::graphics::OpenGL::clear_buffers();

    draw_moon();
    draw_space_station();
    draw_space_craft();
    draw_skybox();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->get_cursor_status()) {
        draw_gui();
    }


    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("framebuffer_rectangle");
    shader->use();
    shader->set_int("screenTexture", 0);

    m_framebuffer->draw_framebuffer_rectangle();
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
    auto camera   = graphics->camera();
    if (platform->get_cursor_status())
        return;

    float delta = platform->dt();
    if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
        camera->move_camera(engine::graphics::Camera::FORWARD, delta);
    }
    if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
        camera->move_camera(engine::graphics::Camera::LEFT, delta);
    }
    if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
        camera->move_camera(engine::graphics::Camera::BACKWARD, delta);
    }
    if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
        camera->move_camera(engine::graphics::Camera::RIGHT, delta);
    }
}
