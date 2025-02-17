//
// Created by marko on 2/11/25.
//

#include <engine/platform/PlatformController.hpp>
#include "MoonEvent.hpp"
#include "MainController.hpp"
#include "glm/vec3.hpp"

MoonEvent::MoonEvent() {
    m_moon_state = ON;
    m_brightness = 1.4f;
    m_fade_speed = 0.2f;
    m_moving_speed = 3.f;
    m_e_pressed = false;
    event_active = false;
 }


void MoonEvent::update_moon(float delta_time) {

    auto platform       = engine::core::Controller::get<engine::platform::PlatformController>();
    auto main_controller = platform->get<MainController>();

    switch (m_moon_state) {
    case OFF:
        m_brightness = 0.2f;
        if (event_active) {
            m_moon_state = FADING_IN;
        }else {
            // -10.f for y value is moons off position, slightly bellow space station
            if(main_controller->light.position.y > -15.f)
                main_controller->light.position.y -= m_moving_speed * delta_time;
        }
        break;

    case FADING_IN:
        m_brightness += m_fade_speed * delta_time;
        if (m_brightness >= 1.4f) {
            m_brightness = 1.4f;
            m_moon_state = ON;
            event_active = false;
        }
        break;

    case ON:
        m_brightness = 1.4f;
        if (event_active) {
            m_moon_state = FADING_OUT;
        }else {
            if(main_controller->light.position.y < 30.f)
                main_controller->light.position.y += m_moving_speed * delta_time;
        }
        break;

    case FADING_OUT:
        m_brightness -= m_fade_speed * delta_time;
        if (m_brightness <= 0.2f) {
            m_brightness = 0.2f;
            m_moon_state = OFF;
            event_active = false;
        }
        break;
    }

    main_controller->light.intensity = glm::vec3(m_brightness);
}

MoonState MoonEvent::get_moon_state() const {
    return m_moon_state;
}

