//
// Created by marko on 2/5/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>

struct Light{
    glm::vec3 intensity = glm::vec3(1.f);
    glm::vec3 position = glm::vec3(15.f, 30.f, -20.f);
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
    void draw_framebuffer_rectangle();
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
    int moon_event = 2;
    glm::vec3 spacecraft_pos = glm::vec3(0.0f, -2.0f, -30.0f);
    float spacecraft_rotation = glm::radians(0.f);


    unsigned int framebuffer;
    unsigned int vao, vbo;// rectangle buff/att objects
    unsigned int texture_colorbuffer; // color attachment
    unsigned int rbo; // render buffer object
    float vertices[24] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    }; // vertices for framebuffer rectangle
};


#endif //MAINCONTROLLER_HPP


