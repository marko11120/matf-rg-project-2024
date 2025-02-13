//
// Created by marko on 2/11/25.
//

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <engine/graphics/OpenGL.hpp>

class Framebuffer {
public:
    Framebuffer();
    void bind();
    void unbind();
    void draw_framebuffer_rectangle();
private:
    unsigned int m_vao, m_rbo, m_framebuffer, m_texture_colorbuffer;

};

#endif //FRAMEBUFFER_H
