//
// Created by marko on 2/11/25.
//

#include "../include/Framebuffer.hpp"
#include <../../engine/include/engine/graphics/OpenGL.hpp>
#include <../../engine/include/engine/platform/PlatformController.hpp>


Framebuffer::Framebuffer() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    int width = platform->window()->width();
    int height = platform->window()->height();

    unsigned int vao = engine::graphics::OpenGL::configure_framebuffer_rectangle();
    unsigned int framebuffer = engine::graphics::OpenGL::create_framebuffer();
    unsigned int texture_colorbuffer = engine::graphics::OpenGL::create_color_attachment(width, height);
    unsigned int rbo = engine::graphics::OpenGL::create_render_buffer(width, height);

    m_framebuffer = framebuffer;
    m_texture_colorbuffer = texture_colorbuffer;
    m_vao = vao;
    m_rbo = rbo;
}

void Framebuffer::bind() {
    engine::graphics::OpenGL::bind_framebuffer(m_framebuffer);
}

void Framebuffer::unbind() {
    engine::graphics::OpenGL::bind_framebuffer(0);
}

void Framebuffer::draw_framebuffer_rectangle() {
    engine::graphics::OpenGL::disable_depth_testing();
    engine::graphics::OpenGL::clear_buffers();

    engine::graphics::OpenGL::bind_buffer(m_vao);
    engine::graphics::OpenGL::draw_arrays(6);
}

void Framebuffer::enable_stencil_testing() {
    engine::graphics::OpenGL::enable_stencil_testing();
}
void Framebuffer::disable_stencil_writing() {
    engine::graphics::OpenGL::disable_stencil_writing();
}

void Framebuffer::stencil_func(std::string func, int ref, unsigned int mask) {
    engine::graphics::OpenGL::stencil_func(func, ref, mask);
}
void Framebuffer::stencil_op(std::string sfail, std::string dfail, std::string dpass) {
    engine::graphics::OpenGL::stencil_op(sfail, dfail, dpass);
}
void Framebuffer::stencil_mask(int mask) {
    engine::graphics::OpenGL::stencil_mask(mask);
}

void Framebuffer::copy_stencil_to_texture() {
    m_stencil_texture = engine::graphics::OpenGL::copy_stencil_to_texture();
}
void Framebuffer::activate_stencil_texture(int slot) {
    engine::graphics::OpenGL::activate_texture(m_stencil_texture, slot);
}

