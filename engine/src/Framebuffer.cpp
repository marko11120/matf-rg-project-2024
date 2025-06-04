//
// Created by marko on 2/11/25.
//

#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>

namespace engine::graphics {

    Framebuffer::Framebuffer(int width, int height, bool g_buffer) {
        m_framebuffer = OpenGL::create_framebuffer();
        if(!g_buffer) {
            m_vao = OpenGL::configure_framebuffer_rectangle();
            m_texture_color_buffer = OpenGL::create_color_attachment(width, height);
            m_rbo = OpenGL::create_render_buffer(width, height);
            m_stencil_texture = OpenGL::create_texture(width, height);
        }else {
            //RG_SHOULD_NOT_REACH_HERE
        }
    }

    void Framebuffer::bind() const{
        OpenGL::bind_framebuffer(m_framebuffer);
    }

    void Framebuffer::unbind() const{
        OpenGL::bind_framebuffer(0);
    }

    void Framebuffer::draw_fullscreen_quad() const{
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

    void Framebuffer::copy_stencil_to_texture() const{
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        int width     = platform->window()->width();
        int height    = platform->window()->height();
        OpenGL::copy_stencil_to_texture(width, height, m_stencil_texture);
    }
    void Framebuffer::activate_stencil_texture(int slot) const{
        OpenGL::activate_texture(m_stencil_texture, slot);
    }

} // namespace engine::graphics
