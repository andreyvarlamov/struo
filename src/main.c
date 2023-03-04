#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define SCREEN_COLS 40
#define SCREEN_ROWS 30
#define SCREEN_TILE_WIDTH 24.0f

// NOTE: Assumes square font in a square atlas
#define ATLAS_COLS 16.0f

typedef GLuint TexID;
typedef GLuint ShaderID;
typedef GLuint VaoID;
typedef GLuint VboID;

static char *_read_file(const char *path)
{
    FILE *f;
    long len;
    char* content;

    f = fopen(path, "rb");
    if (!f)
    {
        printf("Could not open file at %s\n", path);
        exit(-1);
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    content = (char*) calloc(1, len+1);
    fread(content, 1, len, f);
    content[len] = 0; // Null terminate. TODO: Make sure this works properly
    fclose(f);

    return content;
}

static TexID _load_tex_from_file(const char *path)
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

static ShaderID _load_shader_from_file(const char *v_path, const char *f_path)
{
    const char *v_content = _read_file(v_path);
    const char *f_content = _read_file(f_path);

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

static void _key_callback(
    GLFWwindow *window,
    int key,
    int scancode,
    int action,
    int mode
)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

int main()
{
    // OpenGL Init
    // -----------

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

    glfwSetKeyCallback(window, _key_callback);

    glViewport(0, 0, 
        SCREEN_COLS * SCREEN_TILE_WIDTH, SCREEN_ROWS * SCREEN_TILE_WIDTH
    );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Load Resources
    // --------------

    TexID atlas = _load_tex_from_file("res/texture/cp437_rgba.png");
    ShaderID shader = _load_shader_from_file("res/shaders/atlas.vs", "res/shaders/atlas.fs");
    
    // Prepare VAO
    // -----------
    
    VaoID vao;

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

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);

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

    // Provide const shader uniforms
    // -----------------------
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlas);
    glBindVertexArray(vao);

    
    srand(time(NULL));

    int rnds[SCREEN_COLS * SCREEN_ROWS] = {0};
    for (size_t j = 0; j < SCREEN_COLS * SCREEN_ROWS; j++)
    {
        rnds[j] = rand() % 256;
    }

    float delta_time = 0.0f;
    float last_frame = 0.0f;

    float accum = 0.0f;

    // Game Loop
    // ---------
    while(!glfwWindowShouldClose(window))
    {
        // getchar();
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        glfwPollEvents();
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw
        // ----
        accum += delta_time;
        
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

        // ----

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}