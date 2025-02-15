//
// Created by marko on 2/14/25.
//

#ifndef BLOOM_H
#define BLOOM_H

#include "Framebuffer.hpp"

class Bloom {
public:
    static bool bloom;
    static void bloom_color_buffers(Framebuffer* buff);
    static void bind_bloom_textures(Framebuffer* fbuff);
};



#endif //BLOOM_H
