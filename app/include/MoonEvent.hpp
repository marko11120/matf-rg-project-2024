//
// Created by marko on 2/11/25.
//

#ifndef MOONEVENT_H
#define MOONEVENT_H

/**
* @brief enum for states in moon can be
**/
enum MoonState { OFF, FADING_IN, ON, FADING_OUT };

class MoonEvent {
public:
    /**
     * @brief contructor
     */
    MoonEvent();

    /**
     * @brief updates moon based on it state
     * @param delta_time time diff between two frames so the speed doesn't depend on number of frames
     */
    void update_moon(float delta_time);

    /**
     * @brief returns moons state
     */
    MoonState get_moon_state() const;

    /**
     * @brief this var is like semaphore, one event(turn off/one) can not be started if the other one is active
     */
    bool event_active;
private:
    MoonState m_moon_state;
    float m_brightness;
    float m_fade_speed;
    float m_moving_speed;
    bool m_e_pressed = false;
    bool m_should_turn_on = true;
};



#endif //MOONEVENT_H
