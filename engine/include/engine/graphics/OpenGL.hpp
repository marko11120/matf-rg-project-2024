/**
 * @file OpenGL.hpp
 * @brief Defines the OpenGL class that serves as the interface for OpenGL.
 */

#ifndef OPENGL_HPP
#define OPENGL_HPP

#include "Framebuffer.hpp"
#include <engine/resources/Shader.hpp>
#include <cstdint>
#include <filesystem>




namespace engine::resources {
    class Skybox;
}

/**
* @brief Do an error-checked OpenGL call. Throws an OpenGL error if the call fails.
* @param func OpenGL function to call
* @param ... Function arguments
*
* Example:
* @code
* uint32_t texture_id = 0;
* CHECKED_GL_CALL(glGenTextures, 1, &texture_id);
* @endcode
*/
#define CHECKED_GL_CALL(func, ...) engine::graphics::OpenGL::call(std::source_location::current(), func, __VA_ARGS__)

namespace engine::graphics {
    /**
    * @class OpenGL
    * @brief This class serves as the OpenGL interface for your app, since the engine doesn't directly link OpenGL to the app executable.
    *
    * Any OpenGL additional direct OpenGL calls you need should be added here.
    */


    class OpenGL {
    public:
        using ShaderProgramId = uint32_t;

        /**
        * @brief Performs a checked OpenGL call. If the OpenGL call fails, it throws @ref engine::util::EngineError::Type::OpenGLError.

        * @param location Source location of where the call was made.
        * @param glfun OpenGL function to call.
        * @param args  OpenGL function arguments.
        *
        * @returns Return value if the `glfun` has it, otherwise void.
        */
        template<typename TResult, typename... TOpenGLArgs, typename... Args>
        static TResult call(std::source_location location, TResult (*glfun)(TOpenGLArgs...), Args... args) {
            // @formatter:off
            if constexpr (!std::is_same_v<TResult, void>) {
                auto result = glfun(std::forward<Args>(args)...);
                #ifndef NDEBUG
                    assert_no_error(location);
                #endif
                return result;
            } else {
                glfun(std::forward<Args>(args)...);
                #ifndef NDEBUG
                    assert_no_error(location);
                #endif
            }
            // @formatter:on
        }

        /**
        * @brief Converts @ref resources::ShaderType to the OpenGL shader type enum.
        * @returns GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER
        */
        static int32_t shader_type_to_opengl_type(resources::ShaderType type);

        /**
        * @brief Loads the texture from `path` into the OpenGL context.
        *
        * @param path path to a texture file.
        * @param flip_uvs flip_uvs on load.
        * @returns OpenGL id of a texture object.
        */
        static uint32_t generate_texture(const std::filesystem::path &path, bool flip_uvs);

        /**
        * @brief Get texture format for a `number_of_channels`.
        * @param number_of_channels that the texture has.
        * @returns GL_RED, GL_RGB, GL_RGBA for the number_of_channels=[1,3,4] respectively.
        */
        static int32_t texture_format(int32_t number_of_channels);

        /**
        * @brief Initializes the cube Vertex Array Object used for skybox drawing. Caches the vao result.
        * @returns VAO of the cube used for skybox drawing.
        */
        static uint32_t init_skybox_cube();

        /**
        * @brief Check if the shader with the `shader_id` compiled successfully.
        * @returns true if the shader compilation succeeded, false otherwise.
        */
        static bool shader_compiled_successfully(uint32_t shader_id);

        /**
        * @brief Compiles the shader from source.
        * @param shader_source source code for the shader
        * @param shader_type the type of the shader to create1
        * @returns OpenGL context object of the shader.
        */
        static uint32_t compile_shader(const std::string &shader_source,
                                       resources::ShaderType shader_type);

        /**
        * @brief Loads the skybox textures from the `path`.
        * Make sure that images are named: front.jpg, back.jpg, up.jpg, down.jpg, left.jpg, down.jpg.
        * They can be of the other extension as well, but the function will assign each texture to the appropriate
        * side of the cubemap based on the texture file name.
        * @param path directory in which cubemap textures are located.
        * @param flip_uvs wheater to flip_uvs on texture loading.
        * @returns OpenGL id to the cubemap texture
        */
        static uint32_t load_skybox_textures(const std::filesystem::path &path, bool flip_uvs = false);

        /**
        * @brief Enables depth testing.
        */
        static void enable_depth_testing();

        /**
        * @brief Disables depth testing.
        */
        static void disable_depth_testing();

        /**
        * @brief Clears GL_DEPTH_BUFFER_BIT, GL_COLOR_BUFFER_BIT, and GL_STENCIL_BUFFER_BIT.
        */
        static void clear_buffers();
#

        /**
        * @brief creates and bind framebuffer
        * @returns framebuffer id
        */
        static unsigned int create_framebuffer();

        /**
        * @brief creates color attachment and returns its id
        * @param scr_width width of created color attachment
        * @param scr_height height of created color attachment
        * @returns color attachment id
        */
        static unsigned int create_color_attachment(int scr_width, int scr_height);

        /**
        * @brief creates render buffer and returns its id
        * @param scr_width width of render buffer
        * @param scr_height height of render buffer
        * @returns render buffer id
        */
        static unsigned int create_render_buffer(int scr_width, int scr_height);

        /**
        * @brief sends rectangle data on GPU for drawing
        * @returns vao id
        */
        static unsigned int configure_framebuffer_rectangle();

        /**
        * @brief binds frame buffer
        * @param framebuffer id for binding the framebuffer
        */
        static void bind_framebuffer(unsigned int framebuffer);

        /**
        * @brief set OpenGL state color to (r, g, b, a) and clears color buffer
        * @param r percent of red
        * @param g percent of green
        * @param b percent of blue
        * @param a percent of transparency
        */
        static void clear_color(float r, float g, float b, float a);

        /**
        * @brief binds vertex array
        * @param buffer_id id that should be binded
        */
        static void bind_buffer(unsigned int buffer_id);

        /**
        * @brief calls OpenGL glDrawArrays to draw triangles
        * @param vertex_count number of vertexes that should be used for drawing
        */
        static void draw_arrays(int vertex_count);

        /**
        * @brief creates texture
        * @param width width of texture
        * @param height height of texture
        * @returns returns texture id
        */
        static unsigned int create_texture(int width, int height);

        /**
        * @brief activate texture slot and binds texture
        * @param texture id of texture that should be activated
        * @param slot slot on which texture should be avaliable
        */
        static void activate_texture(unsigned int texture, int slot);


        /**
        * @brief creates hdr texture and attaches it to color attachment number attachment_number
        * @param scr_height screen height
        * @param scr_width screen width
        * @param attachment_number as which number should by attached to framebuffer
        * @returns color buffer id
        */
        static unsigned int create_and_attach_color_buffer(int scr_width, int scr_height, int attachment_number);

        /**
        * @brief MRT(multiple render targets) for number_of_attachments color attachments
        * @param number_of_attachments how many attachments should be set to draw into
        */
        static void draw_mrt(int number_of_attachments);
        /**
        * @brief Retrieve the shader compilation error log message.
        * @param shader_id Shader id for which the compilation failed.
        * @returns shader compilation error message.
        */
        static std::string get_compilation_error_message(uint32_t shader_id);

        /**
         * @brief create color buffer primary configured for deferred shading use
         * @param scr_width width of the buffer
         * @param scr_height height of the buffer
         * @param floating_point whether to set the format of the buffer to use floating point or default OpenGL clamp
         * [0, 1]
         * @param buffer_number the sequence number of the attachment
         * @returns returns buffer id
         */
        static unsigned int create_color_buffer(int scr_width, int scr_height, bool floating_point, int buffer_number);

        /**
         * @brief creates 1x1 quad necessary for deferred shading
         * @returns returns vao(vertex array object) id
         */
        static unsigned int create_1x1_quad();

        /**
         * @brief renders a 1x1 quad using triangle strip
         */
        static void render_quad(unsigned int quad_vao);

        /**
        * @brief renders a 1x1 quad using triangle strip
        * @param scr_width width of the buffer
        * @param scr_height height of the buffer
        * @returns buffer id
        */
        static unsigned int create_and_attach_depth_buffer(int scr_width, int scr_height);


        /**
        * @brief bind framebuffer for reading
        * @param framebuffer id of framebuffer
        */
        static void bind_framebuffer_reading(unsigned int framebuffer);

        /**
        * @brief bind framebuffer for drawing
        * @param framebuffer id of framebuffer
        */
        static void bind_framebuffer_drawing(unsigned int framebuffer);

        /**
        * @brief copy the depth buffer from the currently bound framebuffer to the default framebuffer, then bind the default framebuffer for rendering
        * @param scr_width width of the buffer
        * @param scr_height height of the buffer
        */
        static void blit_to_default_framebuffer(int SCR_WIDTH, int SCR_HEIGHT);

        /**
        * @brief copies geometry data so skybox is drawn behind other objects on the scene
        * @param g_buffer_id depth buffer source copy
        * @param framebuffer_id depth buffer dest copy
        * @param width width of the buffer
        * @param height height of the buffer
        */
        static void prepare_for_background_draw(unsigned int g_buffer_id, unsigned int framebuffer_id, int width, int height);

        /**
        * @brief restore framebuffer config set in the prepare_for_background_draw function, like clean up function
        */
        static void finalize_background_draw();

    private:
        /**
        * @brief Throws an engine::util::EngineError of type @ref engine::util::EngineError::Type::OpenGLError if an OpenGL error occurred. Used internally.
        * @param location Source location from where the OpenGL call was made.
        */
        static void assert_no_error(std::source_location location);
    };
}
#endif //OPENGL_HPP
