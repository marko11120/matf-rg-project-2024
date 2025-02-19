//
// Created by marko on 2/14/25.
//

#ifndef BLOOM_H
#define BLOOM_H


namespace engine::graphics {
    class Bloom {
    public:
        /**
        * @brief bool variable that tells whether bloom effect is turned on or off
        **/
        static bool bloom;

        /**
        * @brief  create hdr color buffer and attaches it to active framebuffer
        * @param attachment_number attaches it as this number
        * @param scr_width width of color buffer
        * @param scr_height height of color buffer
        * @returns id of created hdr color buffer
        **/
        static unsigned int create_hdr_color_buffer(unsigned int scr_width, unsigned int scr_height, unsigned int attachment_number);

        /**
         * @brief sets mrt drawing
         * @param textures_number render into this number of textures
         */
      static void mrt(unsigned int texture_number);

        /**
        * @brief binds and activates textures on slots one and two
        * @param fbuff array of texture ids
        **/
        static void bind_bloom_textures(unsigned int* color_buffers);
    };
}

#endif //BLOOM_H
