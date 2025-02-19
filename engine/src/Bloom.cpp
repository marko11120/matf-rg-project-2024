//
// Created by marko on 2/14/25.
//

#include <engine/graphics/Bloom.hpp>
#include <engine/graphics/OpenGL.hpp>

namespace engine::graphics {
    class Framebuffer;

    unsigned int Bloom::create_hdr_color_buffer(unsigned int scr_width, unsigned int scr_height, unsigned int attachment_number) {

        unsigned int id = OpenGL::create_color_buffer(scr_width, scr_height, attachment_number);

        return id;
    }

    void Bloom::mrt(unsigned int texture_number) {
        OpenGL::draw_mrt(texture_number);
    }

    void Bloom::bind_bloom_textures(unsigned int* color_buffers) {
        OpenGL::activate_texture(color_buffers[0], 0);
        OpenGL::activate_texture(color_buffers[1], 1);
    }

    bool Bloom::bloom = true;
}