#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <MainController.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include "MoonEvent.hpp"
#include "engine/graphics/Bloom.hpp"


class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
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
        // if action is not active, then activate it
        if(main_controller->m_moon_event_handler->event_active == false)
            main_controller->m_moon_event_handler->event_active = !main_controller->m_moon_event_handler->event_active;
    } else if (platform->key(engine::platform::KeyId::KEY_UP).is_down()) {
        main_controller->m_spacecraft_pos.y = glm::min(main_controller->m_spacecraft_pos.y + platform->dt() * 3.f, 30.f);
    } else if (platform->key(engine::platform::KeyId::KEY_DOWN).is_down()) {
        main_controller->m_spacecraft_pos.y = glm::max(main_controller->m_spacecraft_pos.y - platform->dt() * 3.f, -2.f);
    } else if (platform->key(engine::platform::KeyId::KEY_LEFT).is_down()) {
        main_controller->m_spacecraft_rotation += platform->dt() * 1.f;
    } else if (platform->key(engine::platform::KeyId::KEY_RIGHT).is_down()) {
        main_controller->m_spacecraft_rotation -= platform->dt() * 1.f;
    } else if (platform->key(engine::platform::KeyId::KEY_B).is_down()) {
        engine::graphics::Bloom::bloom = !engine::graphics::Bloom::bloom;
    }
}

void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    auto observer = std::make_unique<MainPlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(
            std::move(observer));
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->set_cursor_visible(false);

    m_framebuffer = new engine::graphics::Framebuffer();
    m_moon_event_handler           = new MoonEvent();
    engine::graphics::Bloom::bloom = true;

    // m_framebuffer->enable_stencil_testing();
    // m_framebuffer->stencil_func("equal", 1, 0XFF);
    // m_framebuffer->stencil_op("zero", "keep", "replace");

    m_framebuffer->bind();
    engine::graphics::Bloom::bloom_color_buffers(m_framebuffer);
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

void MainController::draw_meteors() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader    = resources->shader("meteor");
    auto model1 = resources->model("meteor1"); // smaller one
    auto model2 = resources->model("meteor2"); // biger one
    auto graphics                     = engine::core::Controller::get<engine::graphics::GraphicsController>();

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("cameraPos", graphics->camera()->Position);

    shader->set_vec3("pointLight.ambient", m_light.ambient);
    shader->set_vec3("pointLight.diffuse", m_light.diffuse);
    shader->set_vec3("pointLight.specular", m_light.specular);
    shader->set_vec3("pointLight.position", m_light.position);
    shader->set_vec3("pointLight.intensity", m_light.intensity);
    shader->set_float("pointLight.linearC", 0.003f);
    shader->set_float("pointLight.quadraticC", 0.0001f);
    shader->set_float("pointLight.shininess", 32.f);

    shader->set_vec3("spotLight.direction", graphics->camera()->Front);
    shader->set_float("spotLight.cut_off", m_spot_light.cut_off);
    shader->set_float("spotLight.outerCut_off", m_spot_light.outterCut_off);
    shader->set_vec3("spotLight.diffuse", m_spot_light.diffuse);
    shader->set_vec3("spotLight.specular", m_spot_light.specular);
    shader->set_float("spotLight.linearC", 0.003f);
    shader->set_float("spotLight.quadraticC", 0.0001f);
    shader->set_float("spotLight.shininess", 32.f);

    static glm::vec3 positions [] = {
        glm::vec3(-29.0f,  -17.0f, -28.0f), // first three positiions are for bigger meteor
        glm::vec3(10.0f, 10.0f, -38.0f),
        glm::vec3(20.0f, 5.0f, -10.0f),
        glm::vec3(-14.0f, 8.0f, -10.0f),  // second three positions are for smaller meteor
        glm::vec3(13.0f, -7.0f, -5.0f),
        glm::vec3(0.0f, -10.0f, -8.0f)
    };

    glm::mat4 model_matrix;

    auto platform                     = engine::core::Controller::get<engine::platform::PlatformController>();
    for(int i = 0; i < 3; i++) {
        model_matrix = glm::mat4(1.f);
        model_matrix = glm::translate(model_matrix, positions[i]);
        float time_value = 3*platform->frame_time().current;
        model_matrix = glm::rotate(model_matrix, glm::radians(time_value), glm::vec3(1.f, 1.f, 1.f));
        model_matrix = glm::scale(model_matrix, glm::vec3(0.3f));
        shader->set_mat4("model", model_matrix);
        model2->draw(shader);
        model_matrix = glm::mat4(1.f);
        model_matrix = glm::translate(model_matrix, positions[i+3]); // i + 3 for second three positions
        time_value = 3*platform->frame_time().current;
        model_matrix = glm::rotate(model_matrix, glm::radians(time_value), glm::vec3(1.f, 1.f, 1.f));
        shader->set_mat4("model", model_matrix);
        model1->draw(shader);
    }
}


void MainController::draw_moon() {
    auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics                     = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto model   = resources->model("Moon");
    auto shader = resources->shader("moon");
    auto platform                     = engine::core::Controller::get<engine::platform::PlatformController>();
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    float time_value = 2*platform->frame_time().current;
    m_moon_event_handler->update_moon(platform->dt());

    shader->set_vec3("light_intensity", m_light.intensity);
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix           = glm::translate(model_matrix, m_light.position);
    model_matrix           = glm::rotate(model_matrix, glm::radians(time_value), glm::vec3(1.0f, 1.0f, 0.0f));
    model_matrix           = glm::scale(model_matrix, glm::vec3(0.6f));
    shader->set_mat4("model", model_matrix);
    model->draw(shader);
}

void MainController::draw_space_station() {
    auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics                     = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *model   = resources->model("space_station");
    engine::resources::Shader *shader = resources->shader("space_objects");

    shader->use();
    shader->set_vec3("pointLight.ambient", m_light.ambient);
    shader->set_vec3("pointLight.diffuse", m_light.diffuse);
    shader->set_vec3("pointLight.specular", m_light.specular);
    shader->set_vec3("pointLight.position", m_light.position);
    shader->set_vec3("pointLight.intensity", m_light.intensity);
    shader->set_float("pointLight.linearC", 0.003f);
    shader->set_float("pointLight.quadraticC", 0.0001f);
    shader->set_float("pointLight.shininess", 32.f);

    shader->set_vec3("spotLight.direction", graphics->camera()->Front);
    shader->set_float("spotLight.cut_off", m_spot_light.cut_off);
    shader->set_float("spotLight.outerCut_off", m_spot_light.outterCut_off);
    shader->set_vec3("spotLight.diffuse", m_spot_light.diffuse);
    shader->set_vec3("spotLight.specular", m_spot_light.specular);
    shader->set_float("spotLight.linearC", 0.003f);
    shader->set_float("spotLight.quadraticC", 0.0001f);
    shader->set_float("spotLight.shininess", 32.f);


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
    engine::resources::Shader *shader = resources->shader("space_objects");

    shader->use();
    shader->set_vec3("pointLight.ambient", m_light.ambient);
    shader->set_vec3("pointLight.diffuse", m_light.diffuse);
    shader->set_vec3("pointLight.specular", m_light.specular);
    shader->set_vec3("pointLight.position", m_light.position);
    shader->set_vec3("pointLight.intensity", m_light.intensity);
    shader->set_float("pointLight.linearC", 0.003f);
    shader->set_float("pointLight.quadraticC", 0.0001f);
    shader->set_float("pointLight.shininess", 32.f);

    shader->set_vec3("spotLight.direction", graphics->camera()->Front);
    shader->set_float("spotLight.cut_off", m_spot_light.cut_off);
    shader->set_float("spotLight.outerCut_off", m_spot_light.outterCut_off);
    shader->set_vec3("spotLight.diffuse", m_spot_light.diffuse);
    shader->set_vec3("spotLight.specular", m_spot_light.specular);

    shader->set_float("spotLight.linearC", 0.003f);
    shader->set_float("spotLight.quadraticC", 0.0001f);
    shader->set_float("spotLight.shininess", 32.f);

    shader->set_vec3("cameraPos", graphics->camera()->Position);

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix           = glm::translate(model_matrix, m_spacecraft_pos);
    model_matrix           = glm::rotate(model_matrix, m_spacecraft_rotation, glm::vec3(0.f, 1.f, 0.f));
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
    m_framebuffer->bind();
    engine::graphics::OpenGL::enable_depth_testing();
    engine::graphics::OpenGL::clear_buffers();


    // setting mask arg to zero so ref and buff value would be masked to zero
    // m_framebuffer->disable_stencil_writing();

    draw_meteors();
    draw_moon();
    draw_space_station();
    draw_space_craft();
    draw_skybox();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->get_cursor_status()) {
        draw_gui();
    }

    // writing ones in buffer for all fragments that will be rendered
    //m_framebuffer->stencil_func("always", 1, 0xFF);
    //m_framebuffer->stencil_mask(0xFF);

    m_framebuffer->unbind();
    engine::graphics::Bloom::bind_bloom_textures(m_framebuffer);

    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("bloom_shader");

    // copying stencil buffer to texture so we can use it in shader
    //m_framebuffer->copy_stencil_to_texture();
    //activate slot 1 and bind stencil_texture to that slot
    //m_framebuffer->activate_stencil_texture(1);

    shader->use();
    shader->set_int("bloomSwitch", (engine::graphics::Bloom::bloom ? 1 : 0));
    shader->set_int("screenTexture", 0);
    shader->set_int("bloomTexture", 1);
    shader->set_float("exposure", m_exposure);

    // using slot 1 for stencilTexture
    //shader->set_int("stencilTexture", 1);

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