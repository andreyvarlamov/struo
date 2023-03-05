#include "main.h"

typedef struct RenderData
{
    TexID atlas;
    ShaderID shader;
    VaoID vao;
} RenderData;

global_variable RenderData _render_data;

GLFWwindow *render_init_window(GLFWkeyfun key_callback)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow *window = glfwCreateWindow(
        SCREEN_COLS * SCREEN_TILE_WIDTH,
        SCREEN_ROWS * SCREEN_TILE_WIDTH,
        "struo",
        NULL,
        NULL
    );

    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetKeyCallback(window, key_callback);

    glViewport(0, 0, 
        SCREEN_COLS * SCREEN_TILE_WIDTH, SCREEN_ROWS * SCREEN_TILE_WIDTH
    );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return window;
}

void render_load_resources()
{
    _render_data.atlas = resource_load_texture("res/texture/cp437_rgba.png");
    _render_data.shader = resource_load_shader("res/shaders/atlas.vs", "res/shaders/atlas.fs");
}

void render_init_vao()
{
    VboID vbo;
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
    };

    glGenVertexArrays(1, &_render_data.vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(_render_data.vao);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices), vertices,
        GL_STATIC_DRAW
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 4, GL_FLOAT, GL_FALSE,
        4 * sizeof(float), (void *) 0
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void render_prepare_shader()
{
    glUseProgram(_render_data.shader);

    mat4 projection;
    glm_ortho(
        0.0f,
        ( float ) SCREEN_COLS * SCREEN_TILE_WIDTH,
        ( float ) SCREEN_ROWS * SCREEN_TILE_WIDTH,
        0.0f,
        -1.0f,
        1.0f,
        projection
    );

    glUniformMatrix4fv(
        glGetUniformLocation(_render_data.shader, "projection"),
        1, GL_FALSE, (float *) projection
    );

    glUniform1f(
        glGetUniformLocation(_render_data.shader, "screen_tile_width"),
        SCREEN_TILE_WIDTH
    );

    glUniform1f(
        glGetUniformLocation(_render_data.shader, "atlas_cols"),
        ATLAS_COLS
    );
    
    glUniform1i(glGetUniformLocation(_render_data.shader, "image"), 0);

    glUseProgram(0);
}

void render_prepare_render()
{
    glUseProgram(_render_data.shader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _render_data.atlas);
    glBindVertexArray(_render_data.vao);
}

void render_render_tile(
    vec2 screen_offset,
    Glyph glyph, vec3 fg_color, vec3 bg_color
)
{
    vec2 atlas_offset = {
        (float) (glyph % (Glyph) ATLAS_COLS),
        (float) (glyph / (Glyph) ATLAS_COLS)
    };

    glUniform2fv(
        glGetUniformLocation(_render_data.shader, "screen_offset"),
        1, screen_offset
    );

    glUniform2fv(
        glGetUniformLocation(_render_data.shader, "atlas_offset"),
        1, atlas_offset
    );

    glUniform3fv(
        glGetUniformLocation(_render_data.shader, "fg_color"),
        1, fg_color
    );

    glUniform3fv(
        glGetUniformLocation(_render_data.shader, "bg_color"),
        1, bg_color
    );

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render_clean_render()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}