//
// Created by marko on 2/11/25.
//

#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>

namespace engine::graphics {
    Framebuffer::Framebuffer() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

        int width = platform->window()->width();
        int height = platform->window()->height();

        unsigned int vao = OpenGL::configure_framebuffer_rectangle();
        unsigned int framebuffer = OpenGL::create_framebuffer();
        unsigned int texture_color_buffer = OpenGL::create_color_attachment(width, height);
        unsigned int rbo = OpenGL::create_render_buffer(width, height);
        unsigned int m_stencil_texture = OpenGL::create_texture(width, height);

        m_framebuffer = framebuffer;
        m_texture_color_buffer = texture_color_buffer;
        m_vao = vao;
        m_rbo = rbo;
    }

    void Framebuffer::bind() {
        OpenGL::bind_framebuffer(m_framebuffer);
    }

    void Framebuffer::unbind() {
        OpenGL::bind_framebuffer(0);
    }

    void Framebuffer::draw_framebuffer_rectangle() {
        OpenGL::disable_depth_testing();
        OpenGL::clear_buffers();

        OpenGL::bind_buffer(m_vao);
        OpenGL::draw_arrays(6);
    }

    void Framebuffer::enable_stencil_testing() {
        OpenGL::enable_stencil_testing();
    }
    void Framebuffer::disable_stencil_writing() {
        OpenGL::disable_stencil_writing();
    }

    void Framebuffer::stencil_func(Flags func, int ref, unsigned int mask) {
        OpenGL::stencil_func(func, ref, mask);
    }
    void Framebuffer::stencil_op(Flags sfail, Flags dfail, Flags dpass) {
        OpenGL::stencil_op(sfail, dfail, dpass);
    }
    void Framebuffer::stencil_mask(int mask) {
        OpenGL::stencil_mask(mask);
    }

    void Framebuffer::copy_stencil_to_texture() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        int width     = platform->window()->width();
        int height    = platform->window()->height();
        OpenGL::copy_stencil_to_texture(width, height, m_stencil_texture);
    }
    void Framebuffer::activate_stencil_texture(int slot) {
        OpenGL::activate_texture(m_stencil_texture, slot);
    }
} // namespace engine::graphics
