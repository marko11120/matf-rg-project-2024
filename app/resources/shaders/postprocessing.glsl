//#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}

//#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform sampler2D stencilTexture;
uniform vec3 greyscale;
uniform int bloomSwitch;

const float offset = 1.0 / 300.0;

void main()
{
vec2 offsets[9] = vec2[](
        vec2(-offset, offset), // top/left
        vec2(0.0f, offset), // top-center
        vec2(offset, offset),
        vec2(-offset, 0.0f),
        vec2(0.0f, 0.0f),
        vec2(0.0f, offset),
        vec2(-offset, -offset),
        vec2(0.0f, -offset),
        vec2(offset, -offset)
    );

float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    );

    vec3 sampleTex[9];
    for (int i = 0; i < 9; ++i) {
        sampleTex[i] = vec3(texture(bloomTexture, TexCoords.st + offsets[i]));
    }

    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; ++i) {
        col += sampleTex[i] * kernel[i];
    }

    vec3 screenColor = texture(screenTexture, TexCoords).rgb;

    vec3 result = screenColor * greyscale + bloomSwitch * col;

    FragColor = vec4(result, 1.f);
}