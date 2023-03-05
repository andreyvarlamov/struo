#include "main.h"

typedef enum RunState
{
    INIT,
    AWAITING_INPUT,
    PLAYER_TURN,
    COMP_TURN,
} RunState;

typedef struct GameState
{
    RunState run_state;
    vec3 fg_col[SCREEN_COLS * SCREEN_ROWS];
    vec3 bg_col[SCREEN_COLS * SCREEN_ROWS];
    Glyph glyphs[SCREEN_COLS * SCREEN_ROWS];
    int player_x;
    int player_y;
} GameState;

global_variable GameState _game_state;

void game_update(float dt, int *_new_key)
{
    switch (_game_state.run_state)
    {
        case INIT:
        {
            _game_state.player_x = SCREEN_COLS / 2;
            _game_state.player_y = SCREEN_ROWS / 2;

            for (int i = 0; i <  SCREEN_COLS * SCREEN_ROWS; i++)
            {
                _game_state.glyphs[i] = '#';
                glm_vec3_copy((vec3) { 0.1f, 0.1f, 0.1f }, _game_state.fg_col[i]);
            }

            _game_state.run_state = AWAITING_INPUT;
        } break;

        case AWAITING_INPUT:
        {
            if (*_new_key != GLFW_KEY_UNKNOWN)
            {
                switch (*_new_key)
                {
                    case GLFW_KEY_H:
                    {
                        _game_state.player_x--;
                        _game_state.run_state = PLAYER_TURN;
                    } break;

                    case GLFW_KEY_J:
                    {
                        _game_state.player_y++;
                        _game_state.run_state = PLAYER_TURN;
                    } break;

                    case GLFW_KEY_K:
                    {
                        _game_state.player_y--;
                        _game_state.run_state = PLAYER_TURN;
                    } break;
                    
                    case GLFW_KEY_L:
                    {
                        _game_state.player_x++;
                        _game_state.run_state = PLAYER_TURN;
                    } break;
                }

                *_new_key = GLFW_KEY_UNKNOWN;
            }
        } break;

        case PLAYER_TURN:
        {
            _game_state.run_state = COMP_TURN;
        } break;

        case COMP_TURN:
        {
            _game_state.run_state = AWAITING_INPUT;
        } break;
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

    render_render_tile(
        (vec2) { _game_state.player_x * SCREEN_TILE_WIDTH, _game_state.player_y * SCREEN_TILE_WIDTH },
        0x02,
        GLM_VEC3_ONE, GLM_VEC3_ZERO
    );
}