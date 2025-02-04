//
// Created by marko on 2/2/25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>

struct Light{
    glm::vec3 ambient = glm::vec3(0.8f);
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);
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
};

#endif //MAINCONTROLLER_H
