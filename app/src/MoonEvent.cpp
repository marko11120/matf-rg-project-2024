//
// Created by marko on 2/11/25.
//

#include "MoonEvent.hpp"
#include "glm/vec3.hpp"

MoonEvent::MoonEvent() {
     moon_event = 2;
     m_pass = true;
 }
glm::vec3 MoonEvent::turn_off() {
    return glm::vec3(0.2f, 0.2f, 0.2f);
 }
glm::vec3 MoonEvent::turn_on() {
     return glm::vec3(1.4f);
 }
