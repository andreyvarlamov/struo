#version 330 core

in vec2 TexCoords;
out vec4 Color;

uniform sampler2D image;
uniform float atlas_cols;

uniform vec3 bg_color; 
uniform vec3 fg_color;
uniform vec2 atlas_offset;

void main()
{
    vec4 tex_col = texture(
        image,
        (TexCoords + atlas_offset) / atlas_cols
        // vec2(
        //     (TexCoords.x + atlas_offset.x) / atlas_cols,
        //     (TexCoords.y + atlas_offset.y) / atlas_cols
        // )
    );

    // TODO: Bulletproof this float check
    if (tex_col.a == 0.0f)
    {
        // Alpha -> background
        Color = vec4(bg_color, 1.0f);
    }
    else
    {
        // Opaque -> foreground;
        Color = vec4(fg_color, 1.0f);
    }
}