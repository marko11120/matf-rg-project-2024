//
// Created by marko on 2/14/25.
//

#ifndef BLOOM_H
#define BLOOM_H
#include "Framebuffer.hpp"

namespace engine::graphics {
    class Bloom {
    public:
        /**
        * @brief bool variable that tells whether bloom effect is turned on or off
        **/
        static bool bloom;

        /**
        * @brief  creates two hdr color buffers and store their ids in fbuff->m_color_buffers array
        * @param fbuff instance of framebuffer where we store ids of color buffers
        **/
        static void create_bloom_color_buffers(Framebuffer* fbuff);

        /**
        * @brief binds and activates textures on slots one and two
        * * @param fbuff instance of framebuffer where are texture ids stored
        **/
        static void bind_bloom_textures(Framebuffer* fbuff);
    };
}

#endif //BLOOM_H
