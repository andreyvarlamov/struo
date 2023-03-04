#include "main.h"

void _key_callback(
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
    GLFWwindow *window = render_init_window(_key_callback);
    
    TexID atlas;
    ShaderID shader;
    render_load_resources(&atlas, &shader);
    
    VaoID vao;
    render_init_vao(&vao);

    render_prepare_shader(shader);

    render_prepare_render(shader, atlas, vao);

    srand(time(NULL));

    float delta_time = 0.0f;
    float last_frame = 0.0f;
    while(!glfwWindowShouldClose(window))
    {
#ifdef STEP_THROUGH_GAME_LOOP
        getchar();
#endif
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render_frame(delta_time, shader);

        glfwSwapBuffers(window);
    }

    render_clean_render();

    glfwTerminate();
    return 0;
}