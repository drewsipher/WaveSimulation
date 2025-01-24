
#version 130
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D texture1;

void main() {
    vec2 tex_offset = 1.0 / textureSize(texture1, 0); // gets size of single texel
    // vec3 sample = texture(texture1, TexCoords).rgb; // center texel
    // vec3 laplacian = sample * -4.0;
    // vec3 previous = texture(texture1, TexCoords).rgb;
    // laplacian += texture(texture1, TexCoords + vec2(tex_offset.x, 0.0)).rgb;
    // laplacian += texture(texture1, TexCoords - vec2(tex_offset.x, 0.0)).rgb;
    // laplacian += texture(texture1, TexCoords + vec2(0.0, tex_offset.y)).rgb;
    // laplacian += texture(texture1, TexCoords - vec2(0.0, tex_offset.y)).rgb;

    // FragColor = vec4(((sample*2)-previous)*laplacian, 1.0);
    FragColor = texture(texture1, TexCoords + vec2(tex_offset.x, 0.0)).rgba;
}