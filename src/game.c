#include "main.h"

typedef struct GameState
{
    float accum;
    int rnds[SCREEN_COLS * SCREEN_ROWS];
    vec3 fg_col[SCREEN_COLS * SCREEN_ROWS];
    vec3 bg_col[SCREEN_COLS * SCREEN_ROWS];
    Glyph glyphs[SCREEN_COLS * SCREEN_ROWS];
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

        _game_state.glyphs[i] = (Glyph) _game_state.rnds[i];

        if (i / SCREEN_COLS == 0 || i / SCREEN_COLS == 1)
        {
            _game_state.glyphs[i] = ' ';
        }

        if (i == 0) _game_state.glyphs[i] = 'H';
        else if (i == 1) _game_state.glyphs[i] = 'e';
        else if (i == 2) _game_state.glyphs[i] = 'l';
        else if (i == 3) _game_state.glyphs[i] = 'l';
        else if (i == 4) _game_state.glyphs[i] = 'o';
        else if (i == 5) _game_state.glyphs[i] = ' ';
        else if (i == 6) _game_state.glyphs[i] = 0x01;
        else if (i == 7) _game_state.glyphs[i] = 0x02;
        else if (i == 8) _game_state.glyphs[i] = 0xE0;
        else if (i == 9) _game_state.glyphs[i] = 0xEA;
        else if (i == 10) _game_state.glyphs[i] = ' ';
        else if (i == 11) _game_state.glyphs[i] = '!';
        else if (i == 12) _game_state.glyphs[i] = '1';
        else if (i == 13) _game_state.glyphs[i] = '!';
        else if (i == 14) _game_state.glyphs[i] = '1';
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
            _game_state.glyphs[i],
            _game_state.fg_col[i], _game_state.bg_col[i]
        );
    }
}