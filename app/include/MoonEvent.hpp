//
// Created by marko on 2/11/25.
//

#ifndef MOONEVENT_H
#define MOONEVENT_H

enum MoonState { OFF, FADING_IN, ON, FADING_OUT };

class MoonEvent {
public:
    MoonEvent();
    void update_moon(float delta_time);
    MoonState get_moon_state() const;
    bool event_active;
private:
    MoonState m_moon_state;
    float m_brightness;
    float m_fade_speed; // speed of light fade
    float m_moving_speed; // moon speed
    bool m_e_pressed = false;
    bool m_should_turn_on = true;
    bool should_switch();
};



#endif //MOONEVENT_H
