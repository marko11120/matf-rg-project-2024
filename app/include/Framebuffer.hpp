//
// Created by marko on 2/11/25.
//

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <engine/graphics/OpenGL.hpp>

class Framebuffer {
public:
    Framebuffer();
    unsigned int get_vbo();
    unsigned int get_vao();
    unsigned int get_rbo();
    unsigned int get_framebuffer();
    unsigned int get_texture_colorbuffer();
    void draw_framebuffer_rectangle();
private:
    unsigned int m_vbo, m_vao, m_rbo, m_framebuffer, m_texture_colorbuffer;

};

#endif //FRAMEBUFFER_H
