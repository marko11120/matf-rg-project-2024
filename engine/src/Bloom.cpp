//
// Created by marko on 2/14/25.
//

#include <engine/graphics/Bloom.hpp>
#include <../../engine/include/engine/graphics/OpenGL.hpp>
#include <../../engine/include/engine/platform/PlatformController.hpp>

namespace engine::graphics {
    class Framebuffer;

    void Bloom::create_bloom_color_buffers(Framebuffer* fbuff) {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        int width = platform->window()->width();
        int height = platform->window()->height();

        fbuff->m_color_buffers[0] = OpenGL::create_color_buffer(width, height, 0);
        fbuff->m_color_buffers[1] = OpenGL::create_color_buffer(width, height, 1);

        OpenGL::draw_mrt(2);
    }

    void Bloom::bind_bloom_textures(Framebuffer* fbuff) {
        OpenGL::activate_texture(fbuff->m_color_buffers[0], 0);
        OpenGL::activate_texture(fbuff->m_color_buffers[1], 1);
    }

    bool Bloom::bloom = true;
}