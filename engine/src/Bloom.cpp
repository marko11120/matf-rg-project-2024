//
// Created by marko on 2/14/25.
//

#include <engine/graphics/Bloom.hpp>
#include <../../engine/include/engine/graphics/OpenGL.hpp>
#include <../../engine/include/engine/platform/PlatformController.hpp>

namespace engine::graphics {
    class Framebuffer;

    void engine::graphics::Bloom::bloom_color_buffers(Framebuffer* fbuff) {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        int width = platform->window()->width();
        int height = platform->window()->height();

        fbuff->m_color_buffers[0] = engine::graphics::OpenGL::create_color_buffer(width, height, 0);
        fbuff->m_color_buffers[1] = engine::graphics::OpenGL::create_color_buffer(width, height, 1);

        engine::graphics::OpenGL::draw_mrt(2);
    }

    void engine::graphics::Bloom::bind_bloom_textures(Framebuffer* fbuff) {
        engine::graphics::OpenGL::activate_texture(fbuff->m_color_buffers[0], 0);
        engine::graphics::OpenGL::activate_texture(fbuff->m_color_buffers[1], 1);
    }

    bool Bloom::bloom = true;
}