#include "main.h"

TexID resource_load_texture(const char *path)
{
    int width, height, nr_channels;
    unsigned char *data = stbi_load(path, &width, &height, &nr_channels, 0);

    TexID tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, width, height,
        0, GL_RGBA, GL_UNSIGNED_BYTE,
        data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return tex_id;
}

ShaderID resource_load_shader(const char *v_path, const char *f_path)
{
    const char *v_content = util_read_file(v_path);
    const char *f_content = util_read_file(f_path);

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_content, NULL);
    glCompileShader(vertex);
    int v_success;
    char v_log[1024];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &v_success);
    if (!v_success)
    {
        glGetShaderInfoLog(vertex, 1024, NULL, v_log);
        printf("ERROR::SHADER: Compile-time error: Vertex\n%s\n", v_log);
    }

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &f_content, NULL);
    glCompileShader(frag);
    int f_success;
    char f_log[1024];
    glGetShaderiv(frag, GL_COMPILE_STATUS, &f_success);
    if (!f_success)
    {
        glGetShaderInfoLog(frag, 1024, NULL, f_log);
        printf("ERROR::SHADER: Compile-time error: Fragment\n%s\n", f_log);
    }

    ShaderID shader = glCreateProgram();
    glAttachShader(shader, vertex);
    glAttachShader(shader, frag);
    glLinkProgram(shader);
    int success;
    char log[1024];
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 1024, NULL, log);
        printf("ERROR::SHADER: Link-time error:\n%s\n", log);
    }

    glDeleteShader(vertex);
    glDeleteShader(frag);

    free((void *)v_content);
    free((void *)f_content);

    return shader;
}