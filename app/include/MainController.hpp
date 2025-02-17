//
// Created by marko on 2/5/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>

namespace engine::graphics {
    class Framebuffer;
}
class MoonEvent;

struct PointLight{
    glm::vec3 intensity = glm::vec3(1.4f);
    glm::vec3 position = glm::vec3(25.f, 30.f, -20.f);
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
    PointLight light;
private:
    friend class MainPlatformEventObserver;
    bool loop() override;
    void initialize() override;
    void begin_draw() override;
    void draw() override;
    /**
     * @brief draws full screen rectangle in framebuffer stored into class instance
     */
    void draw_framebuffer_rectangle();

    /**
     * @brief draws skybox
     */
    void draw_skybox();

    /**
     * @brief draws meteors
     */
    void draw_meteors();

    /**
     * @brief draws moon
     */
    void draw_moon();

    /**
     * @brief draws space station
     */
    void draw_space_station();

    /**
     * @brief draws spacecraft
     */
    void draw_spacecraft();

    /**
     * @brief draws GUI
     */
    void draw_gui();

    void end_draw() override;
    void update() override;

    /**
     * @brief updates camera based on platform events
     */
    void update_camera();
    MoonEvent* m_moon_event_handler;
    float m_exposure = 1;
    glm::vec3 m_spacecraft_pos = glm::vec3(0.0f, -2.0f, -30.0f);
    float m_spacecraft_rotation = glm::radians(0.f);
    SpotLight m_spot_light;
    engine::graphics::Framebuffer * m_framebuffer;
};


#endif //MAINCONTROLLER_HPP


