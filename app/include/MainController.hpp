//
// Created by marko on 2/5/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>

struct Light{
    glm::vec3 position = glm::vec3(15.f, 30.f, -50.f);
    glm::vec3 ambient = glm::vec3(0.8f);
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);
};

struct SpotLight {
    glm::vec3 diffuse = glm::vec3(0.9f);
    glm::vec3 specular = glm::vec3(0.9f);
    float cut_off = cos(glm::radians(12.5f));
    float outterCut_off = cos(glm::radians(17.5f));
};

class MainController : public engine::core::Controller {
public:
    bool loop() override;
    void initialize() override;
    void begin_draw() override;
    void draw() override;
    void draw_skybox();
    void draw_moon();
    void draw_space_station();
    void draw_space_craft();
    void draw_gui();
    void end_draw() override;
    void update() override;
    void update_camera();
    Light light;
    SpotLight spotLight;
    bool moon_event = false;
    std::pair<float, float> moon_position = {15.f, -120.f};
};


#endif //MAINCONTROLLER_HPP


