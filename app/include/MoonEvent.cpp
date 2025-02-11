//
// Created by marko on 2/11/25.
//

#include "MoonEvent.hpp"
#include "glm/vec3.hpp"

glm::vec3 MoonEvent::turn_off() {
    return glm::vec3(0.2);
}

glm::vec3 MoonEvent::turn_on() {
    return glm::vec3(1.0);
}