//
// Created by marko on 2/14/25.
//

#include "../include/Bloom.hpp"
#include <../../engine/include/engine/graphics/OpenGL.hpp>
#include <../../engine/include/engine/platform/PlatformController.hpp>
#include "../include/Framebuffer.hpp"

void Bloom::bloom_color_buffers(Framebuffer* fbuff) {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    int width = platform->window()->width();
    int height = platform->window()->height();

    engine::graphics::OpenGL::bloom_color_buffers(fbuff->m_color_buffers, width, height);
}

void Bloom::bind_bloom_textures(Framebuffer* fbuff) {
    engine::graphics::OpenGL::activate_texture(fbuff->m_color_buffers[0], 0);
    engine::graphics::OpenGL::activate_texture(fbuff->m_color_buffers[1], 1);
}

int Bloom::bloom = 2;