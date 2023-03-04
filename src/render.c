#include "main.h"

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

void render_load_resources(TexID *atlas, ShaderID *shader)
{
    *atlas = fio_load_tex_from_file("res/texture/cp437_rgba.png");
    *shader = fio_load_shader_from_file("res/shaders/atlas.vs", "res/shaders/atlas.fs");
}

void render_init_vao(VaoID *vao)
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

    glGenVertexArrays(1, vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(*vao);

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

void render_prepare_shader(ShaderID shader)
{
    glUseProgram(shader);

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
        glGetUniformLocation(shader, "projection"),
        1, GL_FALSE, (float *) projection
    );

    glUniform1f(
        glGetUniformLocation(shader, "screen_tile_width"),
        SCREEN_TILE_WIDTH
    );

    glUniform1f(
        glGetUniformLocation(shader, "atlas_cols"),
        ATLAS_COLS
    );
    
    glUniform1i(glGetUniformLocation(shader, "image"), 0);

    glUseProgram(0);
}

void render_prepare_render(ShaderID shader, TexID atlas, VaoID vao)
{
    glUseProgram(shader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlas);
    glBindVertexArray(vao);
}

void render_frame(float dt, ShaderID shader)
{
    local_persist float accum = 0.0f; 
    local_persist int rnds[SCREEN_COLS * SCREEN_ROWS] = {0};
    accum += dt;
        
    if (accum > 0.2)
    {
        for (size_t j = 0; j < SCREEN_COLS * SCREEN_ROWS; j++)
        {
            rnds[j] = rand() % 256;
        }

        accum = 0.0;
    }


    for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS; i++)
    {
        float color_offset = (float)rnds[i];
        float f_r = sin(glfwGetTime() + color_offset          ) / 2.0f + 0.5f;
        float f_g = sin(glfwGetTime() + color_offset + 2.09439) / 2.0f + 0.5f;
        float f_b = sin(glfwGetTime() + color_offset + 2.09439 * 2) / 2.0f + 0.5f;
        glUniform3fv(
            glGetUniformLocation(shader, "fg_color"),
            1, (vec3) { f_r, f_g, f_b }
        );
        float b_r = cos(glfwGetTime() + color_offset          ) / 2.0f + 0.5f;
        float b_g = cos(glfwGetTime() + color_offset + 2.09439) / 2.0f + 0.5f;
        float b_b = cos(glfwGetTime() + color_offset + 2.09439 * 2) / 2.0f + 0.5f;
        glUniform3fv(
            glGetUniformLocation(shader, "bg_color"),
            1, (vec3) { b_r, b_g, b_b }
        );

        vec2 screen_offset ={
            (i % SCREEN_COLS) * SCREEN_TILE_WIDTH,
            (i / SCREEN_COLS) * SCREEN_TILE_WIDTH
        };
        glUniform2fv(
            glGetUniformLocation(shader, "screen_offset"),
            1, screen_offset
        );

        vec2 atlas_offset = {
            (float) (rnds[i] % (int) ATLAS_COLS),
            (float) (rnds[i] / (int) ATLAS_COLS)
        };
        glUniform2fv(
            glGetUniformLocation(shader, "atlas_offset"),
            1, atlas_offset
        );

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void render_clean_render()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}