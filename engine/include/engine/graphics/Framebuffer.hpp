//
// Created by marko on 2/11/25.
//

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <vector>

/**
 * @brief enum with all GLenum values for computing stencil buffer functions in OpenGL
 **/
enum Flags{
    NEVER,
    ALWAYS,
    LESS,
    EQUAL,
    NOTEQUAL,
    GREATER,
    KEEP,
    ZERO,
    REPLACE,
    INCR,
    INVERT,
    DECR
};

namespace engine::graphics {
    class Framebuffer {
    public:
        /**
        * @brief constructor, initialize all private variables by calling appropriate functions
        **/
        Framebuffer(int width, int height);

        /**
        * @brief binds framebuffer
        **/
        void bind() const;

        /**
        * @brief unbinds framebuffer
        **/
        void unbind() const;

        /**
        * @brief disables depth testing and draws fullscreen quad, necessary for postprocessing, into active framebuffer
        **/
        void draw_fullscreen_quad() const;

        /**
        * @brief copies stencil buffer content into texture with m_texture_color_buffer id
        **/
        void copy_stencil_to_texture() const;

        /**
        * @brief activates stencil texture with id m_stencil_texture
        * @param slot slot on which it will bind texture
        **/
        void activate_stencil_texture(int slot) const;

        /**
        * @brief enables stencil testing
        **/
        static void enable_stencil_testing();

        /**
        * @brief sets stencil mask to zero so writing into stencil buffer would be omitted
        **/
        static void disable_stencil_writing();

        /**
        * @brief determines the way of writing into stencil buff
        * @param func flag for functions which be used
        * @param ref value that will be tested with stencil buffer value
        * @param mask maks that ANDs ref value and stencil buff value before testing
        **/
        static void stencil_func(Flags func, int ref, unsigned int mask);

        /**
        * @brief actions that should be executed based on testing results
        * @param sfail action if stencil test fails
        * @param dfail aciton if stencil test pass and depth test fails
        * @param dpass action if both tests pass
        */
        static void stencil_op(Flags sfail, Flags dfail, Flags dpass);

        /**
        * @brief sets stencil mask to mask
        */
        static void stencil_mask(int mask);

        std::vector<unsigned int> color_buffers;
    private:
        unsigned int m_vao, m_rbo, m_framebuffer, m_texture_color_buffer, m_stencil_texture;
    };
}

#endif //FRAMEBUFFER_H
