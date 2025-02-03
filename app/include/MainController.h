//
// Created by marko on 2/2/25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include <engine/core/Controller.hpp>

class MainController : public engine::core::Controller {
    bool loop() override;
    void initialize() override;
    void begin_draw() override;
    void draw() override;
    void draw_skybox();
    void draw_moon();
    void draw_space_station();
    void end_draw() override;
    void update() override;
    void update_camera();
};


#endif //MAINCONTROLLER_H
