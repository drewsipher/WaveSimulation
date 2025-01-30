#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D currentWave;
uniform sampler2D speedValues;
uniform sampler2D sources;

void main() {
    float waveHeight = texture(currentWave, TexCoords).r;
    float speed = texture(speedValues, TexCoords).r;
    vec3 source = texture(sources, TexCoords).rgb;

    float red = clamp(waveHeight, 0.0, 1.0) + source.r;
    float green = (1.0-(speed/343.0)) + source.r;
    float blue = clamp(-waveHeight, 0.0, 1.0) + source.r;

    FragColor = vec4(red, green, blue, 1.0);
}