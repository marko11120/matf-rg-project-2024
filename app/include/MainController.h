//
// Created by marko on 2/2/25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include <engine/core/Controller.hpp>

class MainController : public engine::core::Controller {
    bool loop() override;
    void initialize() override;
};

#endif //MAINCONTROLLER_H
