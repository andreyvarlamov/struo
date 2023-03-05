#include "main.h"

typedef struct GameState
{
    float accum;
    int rnds[SCREEN_COLS * SCREEN_ROWS];
    vec3 fg_col[SCREEN_COLS * SCREEN_ROWS];
    vec3 bg_col[SCREEN_COLS * SCREEN_ROWS];
    vec2 atlas_offset[SCREEN_COLS * SCREEN_ROWS];
} GameState;

GameState _game_state;

void game_update(float dt)
{
    _game_state.accum += dt;
    if (_game_state.accum > 0.2)
    {
        for (size_t i = 0; i < SCREEN_COLS * SCREEN_ROWS; i++)
        {
            _game_state.rnds[i] = rand() % 256;
        }

        _game_state.accum = 0.0;
    }

    for (size_t i = 0; i < SCREEN_COLS * SCREEN_ROWS; i++)
    {
        float color_offset = (float)_game_state.rnds[i];

        float f_r = sin(glfwGetTime() + color_offset          ) / 2.0f + 0.5f;
        float f_g = sin(glfwGetTime() + color_offset + 2.09439) / 2.0f + 0.5f;
        float f_b = sin(glfwGetTime() + color_offset + 2.09439 * 2) / 2.0f + 0.5f;
        glm_vec3_copy((vec3) { f_r, f_g, f_b }, _game_state.fg_col[i]);
        
        float b_r = cos(glfwGetTime() + color_offset          ) / 2.0f + 0.5f;
        float b_g = cos(glfwGetTime() + color_offset + 2.09439) / 2.0f + 0.5f;
        float b_b = cos(glfwGetTime() + color_offset + 2.09439 * 2) / 2.0f + 0.5f;
        glm_vec3_copy((vec3) { b_r, b_g, b_b }, _game_state.bg_col[i]);
        
        vec2 atlas_offset = {
            (float) (_game_state.rnds[i] % (int) ATLAS_COLS),
            (float) (_game_state.rnds[i] / (int) ATLAS_COLS)
        };
        glm_vec2_copy(atlas_offset, _game_state.atlas_offset[i]);
    }
}

void game_render(float dt)
{
    for (size_t i = 0; i < SCREEN_COLS * SCREEN_ROWS; i++)
    {
        vec2 screen_offset ={
            (i % SCREEN_COLS) * SCREEN_TILE_WIDTH,
            (i / SCREEN_COLS) * SCREEN_TILE_WIDTH
        };

        render_render_tile(
            screen_offset,
            _game_state.atlas_offset[i],
            _game_state.fg_col[i], _game_state.bg_col[i]
        );
    }
}