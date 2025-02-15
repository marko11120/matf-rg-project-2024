//
// Created by marko on 2/11/25.
//

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <string>

class Framebuffer {
    friend class Bloom;
public:
    Framebuffer();
    void bind();
    void unbind();
    void draw_framebuffer_rectangle();
    void copy_stencil_to_texture();
    void activate_stencil_texture(int slot);
    static void enable_stencil_testing();
    static void disable_stencil_writing();
    static void stencil_func(std::string func, int ref, unsigned int mask);
    static void stencil_op(std::string sfail, std::string dfail, std::string dpass);
    static void stencil_mask(int mask);

private:
    unsigned int m_vao, m_rbo, m_framebuffer, m_texture_colorbuffer, m_stencil_texture;
    unsigned int m_color_buffers[2];

};

#endif //FRAMEBUFFER_H
