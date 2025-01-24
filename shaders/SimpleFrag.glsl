
#version 130
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D texture1;

void main() {
    vec2 tex_offset = 1.0 / textureSize(texture1, 0); // gets size of single texel
    vec3 result = texture(texture1, TexCoords).rgb * -4.0; // center texel

    result += texture(texture1, TexCoords + vec2(tex_offset.x, 0.0)).rgb;
    result += texture(texture1, TexCoords - vec2(tex_offset.x, 0.0)).rgb;
    result += texture(texture1, TexCoords + vec2(0.0, tex_offset.y)).rgb;
    result += texture(texture1, TexCoords - vec2(0.0, tex_offset.y)).rgb;

    // FragColor = vec4(result, 1.0);
    FragColor = texture(texture1, TexCoords).rgba;
}