//
// Created by marko on 2/11/25.
//

#ifndef MOONEVENT_H
#define MOONEVENT_H
#include "glm/vec3.hpp"

class MoonEvent {
public:
    MoonEvent();
    glm::vec3 turn_on();
    glm::vec3 turn_off();
    int moon_event;
private:
    bool m_pass;
};



#endif //MOONEVENT_H
