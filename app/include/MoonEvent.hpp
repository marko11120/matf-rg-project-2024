//
// Created by marko on 2/11/25.
//

#ifndef MOONEVENT_H
#define MOONEVENT_H
#include "glm/vec3.hpp"

enum MoonState { OFF, FADING_IN, ON, FADING_OUT };



class MoonEvent {
public:
    MoonEvent();
    void update_moon(float delta_time);
    bool event_active;
private:
    MoonState m_moon_state;
    float m_brightness;
    float m_fade_speed; // Brzina fade efekta
    bool m_e_pressed = false;
    bool m_should_turn_on = true;
    bool should_switch();
};



#endif //MOONEVENT_H
