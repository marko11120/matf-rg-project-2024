//
// Created by marko on 5/28/25.
//

#ifndef G_BUFFER_H
#define G_BUFFER_H
#include "Framebuffer.hpp"

namespace engine::graphics {

    class GBuffer : public Framebuffer {
        unsigned int m_g_position, m_g_normal, m_g_albedo_spec, m_rbo_depth_buffer;
    public:
        unsigned int quad_vao;
        /**
         * @brief GBuffer constructor that automatically sets up G-buffer attachments.
         * @param width Width of the framebuffer (usually the screen width).
         * @param height Height of the framebuffer (usually the screen height).
         */
        GBuffer(int width, int height);

        /**
         * @brief Activates all G-buffer textures for use in shaders.
         *        Useful during the lighting pass in deferred rendering.
         */
        void activate_all_textures() const;

        /**
         * @brief Returns the texture ID for the position buffer.
         */
        unsigned int get_position_texture() const { return m_g_position; }

        /**
         * @brief Returns the texture ID for the normal buffer.
         */
        unsigned int get_normal_texture() const { return m_g_normal; }

        /**
         * @brief Returns the texture ID for the albedo + specular buffer.
         */
        unsigned int get_albedo_spec_texture() const { return m_g_albedo_spec; }

    };

}

#endif // GBUFFER_H

