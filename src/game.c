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
    Map map;
    int player_x;
    int player_y;
} GameState;

global_variable GameState _game_state;

void game_try_move_player(int x, int y, int width)
{
    if (!_game_state.map.blocked[util_xy_to_i(x, y, width)])
    {
        _game_state.player_x = x;
        _game_state.player_y = y;
    }
}

void game_update(float dt, int *_new_key)
{
    switch (_game_state.run_state)
    {
        case INIT:
        {
            _game_state.player_x = SCREEN_COLS / 2;
            _game_state.player_y = SCREEN_ROWS / 2;

            map_init(&_game_state.map, SCREEN_COLS, SCREEN_ROWS);

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
                        game_try_move_player(_game_state.player_x - 1, _game_state.player_y, SCREEN_COLS);
                        _game_state.run_state = PLAYER_TURN;
                    } break;

                    case GLFW_KEY_J:
                    {
                        game_try_move_player(_game_state.player_x, _game_state.player_y + 1, SCREEN_COLS);
                        _game_state.run_state = PLAYER_TURN;
                    } break;

                    case GLFW_KEY_K:
                    {
                        game_try_move_player(_game_state.player_x, _game_state.player_y - 1, SCREEN_COLS);
                        _game_state.run_state = PLAYER_TURN;
                    } break;
                    
                    case GLFW_KEY_L:
                    {
                        game_try_move_player(_game_state.player_x + 1, _game_state.player_y, SCREEN_COLS);
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
    // Render map
    // ----------
    for (size_t i = 0; i < SCREEN_COLS * SCREEN_ROWS; i++)
    {
        vec2 screen_offset ={
            (i % SCREEN_COLS) * SCREEN_TILE_WIDTH,
            (i / SCREEN_COLS) * SCREEN_TILE_WIDTH
        };

        render_render_tile(
            screen_offset,
            _game_state.map.glyphs[i],
            _game_state.map.fg_col[i], _game_state.map.bg_col[i]
        );
    }

    render_render_tile(
        (vec2) { _game_state.player_x * SCREEN_TILE_WIDTH, _game_state.player_y * SCREEN_TILE_WIDTH },
        0x02,
        (vec3) { 1.0f, 1.0f, 0.5f }, GLM_VEC3_ZERO
    );
}