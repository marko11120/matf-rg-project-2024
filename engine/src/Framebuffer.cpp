//
// Created by marko on 2/11/25.
//

#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/OpenGL.hpp>

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

} // namespace engine::graphics
