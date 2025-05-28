//
// Created by marko on 5/28/25.
//

#include "G_Buffer.hpp"
#include "OpenGL.hpp"

namespace engine::graphics {


    GBuffer::GBuffer(int width, int height) : Framebuffer(width, height, true) {
        m_g_position       = OpenGL::create_color_buffer(width, height, true, 0);
        m_g_normal         = OpenGL::create_color_buffer(width, height, true, 1);
        m_g_albedo_spec    = OpenGL::create_color_buffer(width, height, false, 2);
        m_rbo_depth_buffer = OpenGL::create_and_attach_depth_buffer(width, height);
        quad_vao         = OpenGL::create_1x1_quad();
    }

    void GBuffer::activate_all_textures() const {
        OpenGL::activate_texture(m_g_position, 0);
        OpenGL::activate_texture(m_g_normal, 1);
        OpenGL::activate_texture(m_g_albedo_spec, 2);
    }
}
