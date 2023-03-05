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
    srand(time(NULL));

    GLFWwindow *window = render_init_window(_key_callback);
    
    render_load_resources();
    render_init_vao();
    render_prepare_shader();
    render_prepare_render();

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

        game_update(delta_time);
        game_render(delta_time);

        glfwSwapBuffers(window);
    }

    render_clean_render();

    glfwTerminate();
    return 0;
}