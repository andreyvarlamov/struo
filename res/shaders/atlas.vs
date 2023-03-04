#version 330 core
layout (location = 0) in vec4 Vertex;

out vec2 TexCoords;

uniform mat4 projection;
uniform float screen_tile_width;

uniform vec2 screen_offset;

void main()
{
    gl_Position = projection * vec4(Vertex.xy * screen_tile_width + screen_offset, 0.0f, 1.0f);
    TexCoords = Vertex.zw;
}