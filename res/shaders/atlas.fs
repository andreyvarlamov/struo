#version 330 core

in vec2 TexCoords;
out vec4 Color;

uniform sampler2D image;
uniform float atlas_cols;

uniform vec3 bg_color; 
uniform vec3 fg_color;
uniform vec2 atlas_offset;
uniform bool grayscale;

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

    if (grayscale)
    {
        // Weighted average that's better for human eye
        // https://learnopengl.com/Advanced-OpenGL/Framebuffers#:~:text=Pretty%20cool%20huh%3F-,Grayscale,-Another%20interesting%20effect
        float average = (0.2126 * Color.r + 0.7152 * Color.g + 0.0722 * Color.b) / 3.0f;
        Color = vec4(average, average, average, 1.0f);
    }
}
