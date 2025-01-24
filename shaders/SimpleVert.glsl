#version 130
in vec2 position;
out vec2 TexCoords;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    TexCoords = (position + 1.0) / 2.0; // Transform to [0, 1] range

}