#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <MainController.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include "MoonEvent.hpp"
#include "engine/graphics/Bloom.hpp"
#include "engine/graphics/G_Buffer.hpp"

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
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = graphics->camera();
    if (platform->get_cursor_status())
        return;

    camera->rotate_camera(position.dx, position.dy);
}

void MainPlatformEventObserver::on_key(engine::platform::Key key) {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto main_controller = engine::platform::PlatformController::get<MainController>();

    if (platform->key(engine::platform::KeyId::KEY_F12).is_down()) {
        m_cursor_visibility = !m_cursor_visibility;
        platform->set_cursor_visible(m_cursor_visibility);
    } else if (platform->key(engine::platform::KeyId::KEY_E).is_down()) {
        // if action is not active, then action can be started
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
    } else if(platform->key(engine::platform::KeyId::KEY_Q).is_down()) {
        if(main_controller->m_moon_event_handler->get_moon_state() == OFF)
            main_controller->m_spotlight_switch = !main_controller->m_spotlight_switch;
    }
}

void MainController::initialize() {
    auto observer = std::make_unique<MainPlatformEventObserver>();
    get<engine::platform::PlatformController>()->register_platform_event_observer(std::move(observer));
    auto platform = get<engine::platform::PlatformController>();
    platform->set_cursor_visible(false);


    auto scr_height = platform->window()->height();
    auto scr_width  = platform->window()->width();
    m_framebuffer = new engine::graphics::Framebuffer(scr_width, scr_height, false);
    m_framebuffer->bind();
    m_framebuffer->color_buffers.push_back(engine::graphics::Bloom::create_hdr_color_buffer(scr_width, scr_height, 0));
    m_framebuffer->color_buffers.push_back(engine::graphics::Bloom::create_hdr_color_buffer(scr_width, scr_height, 1));
    m_framebuffer->unbind();

    m_g_buffer = new engine::graphics::GBuffer(platform->window()->width(), platform->window()->height());

    m_moon_event_handler = new MoonEvent();
    engine::graphics::Bloom::bloom = true;
}

bool MainController::loop() {
    auto platform = get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down())
        return false;
    return true;
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}


void MainController::draw_skybox() {
    auto resources = get<engine::resources::ResourcesController>();
    auto skybox    = resources->skybox("stars_skybox");
    auto shader    = resources->shader("skybox");
    auto graphics  = get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::draw_meteors() const{
    auto resources = get<engine::resources::ResourcesController>();
    auto shader = resources->shader("geometry_pass");
    auto graphics = get<engine::graphics::GraphicsController>();

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    static glm::vec3 positions [] = {
        glm::vec3(-29.0f,  -17.0f, -28.0f), // first three positions are for bigger meteor
        glm::vec3(10.0f, 10.0f, -38.0f),
        glm::vec3(20.0f, 5.0f, -10.0f),
        glm::vec3(-14.0f, 8.0f, -10.0f),  // second three positions are for smaller meteor
        glm::vec3(13.0f, -7.0f, -5.0f),
        glm::vec3(0.0f, -10.0f, -8.0f)
    };

    glm::mat4 model_matrix;
    auto model1 = resources->model("meteor1"); // smaller one
    auto model2 = resources->model("meteor2"); // bigger one

    auto platform = get<engine::platform::PlatformController>();
    for(int i = 0; i < 3; i++) {
        model_matrix = glm::mat4(1.f);
        model_matrix = translate(model_matrix, positions[i]);
        float time_value = 3*platform->frame_time().current;
        model_matrix = rotate(model_matrix, glm::radians(time_value), glm::vec3(1.f, 1.f, 1.f));
        model_matrix = scale(model_matrix, glm::vec3(0.3f));
        shader->set_mat4("model", model_matrix);
        model2->draw(shader);
        model_matrix = glm::mat4(1.f);
        model_matrix = translate(model_matrix, positions[i+3]); // i + 3 for second three positions
        time_value = 3*platform->frame_time().current;
        model_matrix = rotate(model_matrix, glm::radians(time_value), glm::vec3(1.f, 1.f, 1.f));
        shader->set_mat4("model", model_matrix);
        model1->draw(shader);
    }
}

void MainController::draw_moon() const{
    auto resources = get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();
    auto shader = resources->shader("moon");
    auto platform = get<engine::platform::PlatformController>();

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    float time_value = 2*platform->frame_time().current;
    m_moon_event_handler->update_moon(platform->dt());

    shader->set_vec3("spotLight.direction", graphics->camera()->Front);
    shader->set_float("spotLight.cutOff", m_spotlight.cut_off);
    shader->set_float("spotLight.outerCutOff", m_spotlight.outer_cut_off);
    shader->set_vec3("spotLight.diffuse", m_spotlight.diffuse);
    shader->set_vec3("spotLight.specular", m_spotlight.specular);
    shader->set_float("spotLight.linearC", m_spotlight.linear);
    shader->set_float("spotLight.quadraticC", m_spotlight.quadratic);
    shader->set_float("spotLight.shininess", m_spotlight.shininess);
    if(m_moon_event_handler->get_moon_state() == OFF)
        shader->set_int("spotLightSwitch", (m_spotlight_switch ? 1 : 0));


    shader->set_vec3("light_intensity", point_light.intensity);
    shader->set_vec3("cameraPos", graphics->camera()->Position);
    shader->set_float("border", 0.9f);

    auto model_matrix = glm::mat4(1.0f);
    model_matrix = translate(model_matrix, point_light.position);
    model_matrix = rotate(model_matrix, glm::radians(time_value), glm::vec3(1.0f, 1.0f, 0.0f));
    model_matrix = scale(model_matrix, glm::vec3(0.6f));
    shader->set_mat4("model", model_matrix);

    auto model = resources->model("Moon");
    model->draw(shader);
}

void MainController::draw_space_station() const{
    auto resources = get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();
    auto shader = resources->shader("geometry_pass");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    auto model_matrix = glm::mat4(1.0f);
    model_matrix = translate(model_matrix, glm::vec3(0.0f, -7.f, -30.0f));
    model_matrix = scale(model_matrix, glm::vec3(0.08f));
    shader->set_mat4("model", model_matrix);

    auto model = resources->model("space_station");
    model->draw(shader);
}

void MainController::draw_spacecraft() const{
    auto resources = get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();
    auto shader = resources->shader("geometry_pass");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    auto model_matrix = glm::mat4(1.0f);
    model_matrix = translate(model_matrix, m_spacecraft_pos);
    model_matrix = rotate(model_matrix, m_spacecraft_rotation, glm::vec3(0.f, 1.f, 0.f));
    model_matrix = scale(model_matrix, glm::vec3(0.4f));
    shader->set_mat4("model", model_matrix);

    auto model = resources->model("spacecraft2");
    model->draw(shader);
}

void MainController::draw_gui() {
    auto graphics = get<engine::graphics::GraphicsController>();
    graphics->begin_gui();
    {
        ImGui::Begin("Camera info");
        ImGui::SetNextWindowPos(ImVec2(500, 350));
        ImGui::SetNextWindowSize(ImVec2(710, 300));
        auto camera = graphics->camera();
        const auto &c = *camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::SliderFloat("Camera x", &camera->Position.x, -20.f, 20.0f, "%.2f");
        ImGui::SliderFloat("Camera y", &camera->Position.y, -20.f, 20.0f, "%.2f");
        ImGui::SliderFloat("Camera z", &camera->Position.z, -20.f, 20.0f, "%.2f");

        ImGui::Text("Point light sliders:");
        ImGui::Text("Point light ambient intensity: (%f, %f, %f)", point_light.ambient.x, point_light.ambient.y, point_light.ambient.z);
        ImGui::SliderFloat3("Point light ambient slider", glm::value_ptr(point_light.ambient), 0.1f, 1.f);

        ImGui::Text("Point diffuse light intensity: (%f, %f, %f)", point_light.diffuse.x, point_light.diffuse.y, point_light.diffuse.z);
        ImGui::SliderFloat3("Point diffuse slider", glm::value_ptr(point_light.diffuse), 0.1f, 1.f);

        ImGui::Text("Point light specular intensity: (%f, %f, %f)", point_light.specular.x, point_light.specular.y, point_light.specular.z);
        ImGui::SliderFloat3("Point specular slider", glm::value_ptr(point_light.specular), 0.1f, 1.f);

        ImGui::Text("Spot light sliders:");
        ImGui::Text("Spot light diffuse intensity: (%f, %f, %f)", m_spotlight.diffuse.x, m_spotlight.diffuse.y, m_spotlight.diffuse.z);
        ImGui::SliderFloat3("Spot light diffuse slider", glm::value_ptr(m_spotlight.diffuse), 0.1f, 1.f);

        ImGui::Text("Spot light diffuse intensity: (%f, %f, %f)", m_spotlight.specular.x, m_spotlight.specular.y, m_spotlight.specular.z);
        ImGui::SliderFloat3("Spot light specular slider", glm::value_ptr(m_spotlight.specular), 0.1f, 1.f);

        ImGui::Text("Cut off angle: (%f)", m_spotlight.cut_off);
        ImGui::SliderFloat("Cut off slider", &m_spotlight.cut_off, -1.0f, 1.f);

        ImGui::Text("Outer cut off angle: (%f)", m_spotlight.outer_cut_off);
        ImGui::SliderFloat("Outer cut off slider", &m_spotlight.outer_cut_off, -1.0f, 1.f);
        ImGui::End();
    }
    graphics->end_gui();
}

void MainController::geometry_pass_deferred_shading() const{
    draw_meteors();
    draw_space_station();
    draw_spacecraft();
}

void MainController::draw() {
    engine::graphics::OpenGL::enable_depth_testing();

    // geometry pass
    m_g_buffer->bind();
    engine::graphics::OpenGL::clear_buffers();
    engine::graphics::OpenGL::draw_mrt(3);
    geometry_pass_deferred_shading();
    m_g_buffer->unbind();

    // lighting pass
    m_framebuffer->bind();
    engine::graphics::OpenGL::clear_buffers();
    engine::graphics::OpenGL::draw_mrt(m_framebuffer->color_buffers.size());
    auto resources            = get<engine::resources::ResourcesController>();
    auto lighting_pass_shader = resources->shader("lighting_pass");
    lighting_pass_shader->use();
    lighting_pass_shader->set_int("gPosition", 0);
    lighting_pass_shader->set_int("gNormal", 1);
    lighting_pass_shader->set_int("gAlbedoSpec", 2);

    engine::graphics::OpenGL::activate_texture(m_g_buffer->get_position_texture(), 0);
    engine::graphics::OpenGL::activate_texture(m_g_buffer->get_normal_texture(), 1);
    engine::graphics::OpenGL::activate_texture(m_g_buffer->get_albedo_spec_texture(), 2);

    // set up lights
    lighting_pass_shader->set_vec3("pointLight.ambient", point_light.ambient);
    lighting_pass_shader->set_vec3("pointLight.diffuse", point_light.diffuse);
    lighting_pass_shader->set_vec3("pointLight.specular", point_light.specular);
    lighting_pass_shader->set_vec3("pointLight.position", point_light.position);
    lighting_pass_shader->set_vec3("pointLight.intensity", point_light.intensity);
    lighting_pass_shader->set_float("pointLight.linearC", point_light.linear);
    lighting_pass_shader->set_float("pointLight.quadraticC", point_light.quadratic);
    lighting_pass_shader->set_float("pointLight.shininess", point_light.shininess);
    auto graphics = get<engine::graphics::GraphicsController>();
    lighting_pass_shader->set_vec3("spotLight.direction", graphics->camera()->Front);
    lighting_pass_shader->set_float("spotLight.cutOff", m_spotlight.cut_off);
    lighting_pass_shader->set_float("spotLight.outerCutOff", m_spotlight.outer_cut_off);
    lighting_pass_shader->set_vec3("spotLight.diffuse", m_spotlight.diffuse);
    lighting_pass_shader->set_vec3("spotLight.specular", m_spotlight.specular);
    lighting_pass_shader->set_float("spotLight.linearC", m_spotlight.linear);
    lighting_pass_shader->set_float("spotLight.quadraticC", m_spotlight.quadratic);
    lighting_pass_shader->set_float("spotLight.shininess", m_spotlight.shininess);
    if (m_moon_event_handler->get_moon_state() == OFF)
        lighting_pass_shader->set_int("spotLightSwitch", (m_spotlight_switch ? 1 : 0));
    lighting_pass_shader->set_vec3("cameraPos", graphics->camera()->Position);
    lighting_pass_shader->set_float("border", 1.1f);

    engine::graphics::OpenGL::disable_depth_testing();
    engine::graphics::OpenGL::render_quad(m_g_buffer->quad_vao);

    engine::graphics::OpenGL::bind_framebuffer_reading(m_g_buffer->get_framebuffer_id());
    engine::graphics::OpenGL::bind_framebuffer_drawing(0);

    auto platform   = get<engine::platform::PlatformController>();
    auto scr_height = platform->window()->height();
    auto scr_width  = platform->window()->width();
    engine::graphics::OpenGL::blit_to_default_framebuffer(scr_width, scr_height);

    m_framebuffer->bind();
    engine::graphics::OpenGL::enable_depth_testing();

    engine::graphics::OpenGL::prepare_for_background_draw(m_g_buffer->get_framebuffer_id(), m_framebuffer->get_framebuffer_id(), scr_width, scr_height);
    draw_skybox();
    engine::graphics::OpenGL::finalize_background_draw();

    draw_moon();

    m_framebuffer->unbind();

    // finally rendering to the screen
    engine::graphics::OpenGL::disable_depth_testing();
    engine::graphics::OpenGL::clear_buffers();
    engine::graphics::Bloom::activate_bloom_textures(m_framebuffer->color_buffers);
    auto postprocessing_shader = resources->shader("postprocessing");

    postprocessing_shader->use();
    postprocessing_shader->set_int("bloomSwitch", (engine::graphics::Bloom::bloom ? 1 : 0));
    postprocessing_shader->set_int("screenTexture", 0);
    postprocessing_shader->set_int("bloomTexture", 1);

    if (m_moon_event_handler->get_moon_state() == OFF)
        postprocessing_shader->set_vec3("greyscale", glm::vec3(0.299f, 0.587f, 0.114f));
    else if (m_moon_event_handler->get_moon_state() == ON)
        postprocessing_shader->set_vec3("greyscale", glm::vec3(1.f));

    m_framebuffer->draw_fullscreen_quad();


    if (platform->get_cursor_status()) {
        draw_gui();
    }
}

void MainController::end_draw() {
    auto platform = get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

void MainController::terminate() {
    delete m_framebuffer;
    delete m_moon_event_handler;
}


void MainController::update() {
    update_camera();
}

void MainController::update_camera() {
    auto platform = get<engine::platform::PlatformController>();
    auto graphics = get<engine::graphics::GraphicsController>();
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