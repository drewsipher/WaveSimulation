#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D currentWave;
uniform sampler2D speedValues;

void main() {
    float waveHeight = texture(currentWave, TexCoords).r;
    float speed = texture(speedValues, TexCoords).r;
    FragColor = vec4(waveHeight, 1.0-(speed/343.0), waveHeight, 1.0);
}