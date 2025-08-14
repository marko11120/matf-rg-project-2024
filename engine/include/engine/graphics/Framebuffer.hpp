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
        * @param width width of framebuffer that is created
        * @param height height of the framebuffer that is created
        * @param g_buffer type of framebuffer that is created
        **/
        Framebuffer(int width, int height, bool g_buffer);

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
        * @brief returns id of framebuffer
        */
        unsigned int get_framebuffer_id() const {
            return m_framebuffer;
        };

        std::vector<unsigned int> color_buffers;

    private:
        unsigned int m_vao, m_rbo, m_texture_color_buffer, m_stencil_texture, m_framebuffer;
    };
}

#endif //FRAMEBUFFER_H
