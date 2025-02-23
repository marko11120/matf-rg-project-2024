//
// Created by marko on 2/5/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>

namespace engine::graphics {
    class Framebuffer;
    class Bloom;
}
class MoonEvent;

struct PointLight {
    glm::vec3 ambient   = glm::vec3(0.8f);
    glm::vec3 diffuse   = glm::vec3(1.0f);
    glm::vec3 specular  = glm::vec3(1.0f);
    glm::vec3 intensity = glm::vec3(1.4f);
    glm::vec3 position = glm::vec3(25.f, 30.f, -20.f);

    float linear = 0.003f;
    float quadratic = 0.0001f;
    float shininess = 32.0f;
};

struct SpotLight {
    glm::vec3 diffuse = glm::vec3(1.f);
    glm::vec3 specular = glm::vec3(1.f);
    float cut_off = cos(glm::radians(12.5f));
    float outer_cut_off = cos(glm::radians(17.5f));

    float linear = 0.003f;
    float quadratic = 0.0001f;
    float shininess = 32.0f;
};

class MainController : public engine::core::Controller {
public:
    PointLight point_light;
private:
    friend class MainPlatformEventObserver;

    /**
     * @brief determines does render loop should continue or not
     */
    bool loop() override;

    /**
     * @brief initialize main controller
     */
    void initialize() override;

    /**
     * @brief sets up resources for drawing
     */
    void begin_draw() override;

    /**
     * @brief calls other draw functions to draw scene
     */
    void draw() override;

    /**
     * @brief draws skybox
     */
    static void draw_skybox();

    /**
     * @brief draws meteors
     */
    void draw_meteors() const;

    /**
     * @brief draws moon
     */
    void draw_moon() const;

    /**
     * @brief draws space station
     */
    void draw_space_station() const;

    /**
     * @brief draws spacecraft
     */
    void draw_spacecraft() const;

    /**
     * @brief draws GUI
     */
    void draw_gui();

    /**
     * @brief function marks end of drawing, calls swap buffers and deinitialization
     */
    void end_draw() override;

    /**
     * @update update changed states before drawing
     */
    void update() override;

    /**
     * @brief updates camera based on platform events
     */
    void update_camera();

    /**
     * @brief delete memory allocated for drawing resources
     */
    void terminate() override;

    bool m_spotlight_switch = 1;
    MoonEvent* m_moon_event_handler;
    glm::vec3 m_spacecraft_pos = glm::vec3(0.0f, -2.0f, -30.0f);
    float m_spacecraft_rotation = glm::radians(0.f);
    SpotLight m_spotlight;
    engine::graphics::Framebuffer * m_framebuffer;
};


#endif //MAINCONTROLLER_HPP


