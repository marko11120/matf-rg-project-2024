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
    m_e_pressed = false;
    event_active = false;
 }

bool MoonEvent::should_switch() {
    if(event_active && !m_e_pressed) {
        m_e_pressed = true;
        return true;
    }else if(!event_active)
        m_e_pressed = false;

    return false;
}

void MoonEvent::update_moon(float delta_time) {

    switch (m_moon_state) {
    case OFF:
        m_brightness = 0.2f;
        if (event_active) {
            m_moon_state = FADING_IN;
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

    auto platform       = engine::core::Controller::get<engine::platform::PlatformController>();
    auto main_controller = platform->get<MainController>();

    main_controller->m_light.intensity = glm::vec3(m_brightness);
}

