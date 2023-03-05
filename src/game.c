#include "main.h"

typedef enum RunState
{
    INIT,
    AWAITING_INPUT,
    PLAYER_TURN,
    COMP_TURN,
} RunState;

typedef struct Map
{
    vec3 fg_col[SCREEN_COLS * SCREEN_ROWS];
    vec3 bg_col[SCREEN_COLS * SCREEN_ROWS];
    Glyph glyphs[SCREEN_COLS * SCREEN_ROWS];
} Map;

typedef struct GameState
{
    RunState run_state;
    Map map;
    int player_x;
    int player_y;
} GameState;

global_variable GameState _game_state;

void game_init_map(int width, int height)
{
    for (int i = 0; i <  width * height; i++)
    {
        _game_state.map.glyphs[i] = 0xF9;
        glm_vec3_copy((vec3) { 0.05f, 0.05f, 0.05f }, _game_state.map.fg_col[i]);
        glm_vec3_copy(GLM_VEC3_ZERO, _game_state.map.bg_col[i]);
    }

    int border_tiles_count = width * 2 + height * 2 - 4;
    int *fill_indeces = (int *) calloc(1, border_tiles_count * sizeof(int));

    int fill_indeces_iter = 0;

    // Draw horizontal boundaries
    for (int i = 0; i < width; i++)
    {
        fill_indeces[fill_indeces_iter] = i;
        fill_indeces_iter++;
        fill_indeces[fill_indeces_iter] = util_xy_to_i(i, height - 1, width);
        fill_indeces_iter++;
    }
    printf("\n");

    for (int i = 1; i < height - 1; i++)
    {
        fill_indeces[fill_indeces_iter] = util_xy_to_i(0, i, width);
        fill_indeces_iter++;
        fill_indeces[fill_indeces_iter] = util_xy_to_i(width - 1, i, width);
        fill_indeces_iter++;
    }
    for (int i  = 0; i < border_tiles_count; i++)
    {
        _game_state.map.glyphs[fill_indeces[i]] = '#';
        glm_vec3_copy((vec3) { 0.6f, 0.6f, 0.6f }, _game_state.map.fg_col[fill_indeces[i]]);
        glm_vec3_copy((vec3) { 0.7f, 0.7f, 0.7f }, _game_state.map.bg_col[fill_indeces[i]]);
        printf("%d ", fill_indeces[i]);
    }

    free(fill_indeces);

    printf("\n");
}

void game_update(float dt, int *_new_key)
{
    switch (_game_state.run_state)
    {
        case INIT:
        {
            _game_state.player_x = SCREEN_COLS / 2;
            _game_state.player_y = SCREEN_ROWS / 2;

            game_init_map(SCREEN_COLS, SCREEN_ROWS);

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
        GLM_VEC3_ONE, GLM_VEC3_ZERO
    );
}