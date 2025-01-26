#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D currentWave;

void main() {
    float waveHeight = texture(currentWave, TexCoords).r;
    FragColor = vec4(1.0, 1.0, 0.0, 1.0);//vec4(waveHeight, waveHeight, waveHeight, 1.0);
}