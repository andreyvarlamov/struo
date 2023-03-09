#include "main.h"

typedef enum RunState
{
    INIT,
    AWAITING_INPUT,
    COMP_TURN,
} RunState;

typedef struct GameState
{
    Map map;
    bool collisions[SCREEN_COLS * SCREEN_ROWS];
    Entity enemies[ENEMY_NUM];
    Entity player;
    bool player_fov[SCREEN_COLS * SCREEN_ROWS];
    size_t enemy_num;
    RunState run_state;
} GameState;

global_variable GameState _game_state;

bool game_check_move_entity(Point pos, Point *new_out, Direction dir, int map_width)
{
    *new_out = pos;

    switch (dir)
    {
        case DIR_NORTH:
        {
            new_out->y--;
        } break;
        case DIR_EAST:
        {
            new_out->x++;
        } break;
        case DIR_SOUTH:
        {
            new_out->y++;
        } break;
        case DIR_WEST:
        {
            new_out->x--;
        } break;
        default:
        {
            return false;
        } break;
    }

    return !_game_state.collisions[util_p_to_i(*new_out, map_width)];
}

bool game_try_move_entity(Point *pos, Direction dir, int map_width)
{
    Point new = *pos;

    bool did_move = false;
    if (game_check_move_entity(*pos, &new, dir, map_width))
    {
        *pos = new;
        did_move = true;
    }
    else if (util_p_cmp(_game_state.player.pos, new))
    {
        printf("Attacking player.\n");
        // TODO: Attack player
        did_move = true;
    }

    return did_move;
}

bool game_try_move_entity_xy(Point *pos, Point new, int map_width)
{
    bool did_move = false;
    if (!_game_state.collisions[util_p_to_i(new, map_width)])
    {
        pos->x = new.x;
        pos->y = new.y;
        did_move = true;
    }
    else if (util_p_cmp(_game_state.player.pos, new))
    {
        printf("Attacking player.\n");
        // TODO: Attack player
        did_move = true;
    }

    return did_move;
}

void game_update_collisions()
{
    // Refresh collisions based on underlying map
    memcpy(
        &_game_state.collisions,
        &_game_state.map.blocked,
        SCREEN_COLS * SCREEN_ROWS * sizeof(bool)
    );

    // Update based on player's position
    // ---------------------------------
    _game_state.collisions[util_p_to_i(_game_state.player.pos, SCREEN_COLS)] = true;

    // Update based enemies' positions
    // --------------------------------
    for (size_t i = 0; i < _game_state.enemy_num; i++)
    {
        if ( _game_state.enemies[i].alive)
        {
            _game_state.collisions[util_p_to_i(_game_state.enemies[i].pos, SCREEN_COLS)] = true;
        }
    }
}

void game_update(float dt, int *_new_key)
{
    switch (_game_state.run_state)
    {
        case INIT:
        {
            map_gen_level(&_game_state.map, SCREEN_COLS, SCREEN_ROWS);

            // Init player
            // -----------
            _game_state.player.pos = util_xy_to_p(3, 30);
            _game_state.player.glyph = 0x02;
            _game_state.player.alive = true;
            glm_vec3_copy((vec3) { 1.0f, 1.0f, 0.5f }, _game_state.player.fg);
            glm_vec3_copy(GLM_VEC3_ZERO, _game_state.player.bg);

            game_update_collisions();
            entity_calc_player_fov(
                _game_state.map.opaque, SCREEN_COLS, SCREEN_COLS,
                _game_state.player.pos, _game_state.player_fov
            );

            // Init enemies
            // ------------
            _game_state.enemy_num = ENEMY_NUM;
            for (size_t i = 0; i < 10; i++)
            {
                Point p;

                bool found = false;
                int attempts = 20;
                do
                {
                    p.x = rand() % SCREEN_COLS;
                    p.y = rand() % SCREEN_ROWS;
                    found = !_game_state.collisions[util_p_to_i(p, SCREEN_COLS)];
                    attempts--;
                }
                while (!found && attempts >= 0);

                if (found)
                {
                    _game_state.enemies[i].pos = p;
                    _game_state.enemies[i].glyph = 'r';
                    _game_state.enemies[i].alive = true;
                    glm_vec3_copy((vec3) { 1.0f, 0.5f, 0.05f }, _game_state.enemies[i].fg);
                    glm_vec3_copy((vec3) { 0.5f, 0.15f, 0.15f }, _game_state.enemies[i].bg);

                    game_update_collisions();
                }
            }

            _game_state.run_state = AWAITING_INPUT;
        } break;

        case AWAITING_INPUT:
        {
            bool did_move = false;

            if (*_new_key != GLFW_KEY_UNKNOWN)
            {
                switch (*_new_key)
                {
                    case GLFW_KEY_H:
                    {
                        did_move = game_try_move_entity(
                            &_game_state.player.pos,
                            DIR_WEST,
                            SCREEN_COLS
                        );
                    } break;

                    case GLFW_KEY_J:
                    {
                        did_move = game_try_move_entity(
                            &_game_state.player.pos,
                            DIR_SOUTH,
                            SCREEN_COLS
                        );
                    } break;

                    case GLFW_KEY_K:
                    {
                        did_move = game_try_move_entity(
                            &_game_state.player.pos,
                            DIR_NORTH,
                            SCREEN_COLS
                        );
                    } break;
                    
                    case GLFW_KEY_L:
                    {
                        did_move = game_try_move_entity(
                            &_game_state.player.pos,
                            DIR_EAST,
                            SCREEN_COLS
                        );
                    } break;

                    case GLFW_KEY_COMMA:
                    {
                        Point p;

                        int attempts = 20;
                        bool found = true;
                        do
                        {
                            p.x = rand() % SCREEN_COLS;
                            p.y = rand() % SCREEN_ROWS;
                            found = !_game_state.collisions[util_p_to_i(p, SCREEN_COLS)];
                            attempts--;
                        }
                        while (!found && attempts >= 0);

                        if (found)
                        {
                            _game_state.player.pos = p;
                            did_move = true;
                        }
                    } break;

                    case GLFW_KEY_PERIOD:
                    {
                        did_move = true;
                    } break;
                }

                *_new_key = GLFW_KEY_UNKNOWN;
            }

            if (did_move)
            {
                entity_calc_player_fov(
                    _game_state.map.opaque, SCREEN_COLS, SCREEN_ROWS,
                    _game_state.player.pos, _game_state.player_fov
                );
                game_update_collisions();
                _game_state.run_state = COMP_TURN;
            }
        } break;

        case COMP_TURN:
        {
            for (size_t i = 0; i < _game_state.enemy_num; i++)
            {
                if (_game_state.enemies[i].alive)
                {
#if 0
                    bool should_move = (rand() % 4) == 0;
                    if (should_move)
                    {
                        bool did_move = false;
                        do
                        {
                            Direction dir = rand() % 4;

                            did_move = game_try_move_entity(
                                &_game_state.enemies[i].x,
                                &_game_state.enemies[i].y,
                                dir,
                                SCREEN_COLS
                            );
                        }
                        while (!did_move);
                    }
#elif 0
                    Direction dir = DIR_NONE;
                    int min_dist = INT_MAX;

                    for (size_t dir_iter = 0; dir_iter < DIR_NONE; dir_iter++)
                    {
                        int new_x = -1;
                        int new_y = -1;
                        game_check_move_entity(
                            _game_state.enemies[i].x,
                            _game_state.enemies[i].y,
                            &new_x, &new_y,
                            (Direction) dir_iter,
                            SCREEN_COLS
                        );

                        int dist = util_calc_sqr_distance(
                            new_x, new_y,
                            _game_state.player.x, _game_state.player.y
                        );

                        if (dist < min_dist)
                        {
                            dir = (Direction) dir_iter;
                            min_dist = dist;
                        }
                    }

                    if (dir != DIR_NONE)
                    {
                        bool did_move = game_try_move_entity(
                            &_game_state.enemies[i].x,
                            &_game_state.enemies[i].y,
                            dir,
                            SCREEN_COLS
                        );

                        if (did_move)
                        {
                            game_update_collisions();
                        }
                    }
#elif 1
                    if (entity_check_pos_within_fov(
                        _game_state.map.opaque,
                        SCREEN_COLS, SCREEN_ROWS,
                        _game_state.enemies[i].pos,
                        _game_state.player.pos
                    ))
                    {
                        Point next = pathfinding_bfs(
                            _game_state.collisions, SCREEN_COLS, SCREEN_ROWS,
                            _game_state.enemies[i].pos, _game_state.player.pos);

                        bool did_move = false;

                        if (next.x != -1 && next.y != -1)
                        {
                            did_move = game_try_move_entity_xy(
                                &_game_state.enemies[i].pos,
                                next,
                                SCREEN_COLS
                            );
                        }

                        if (did_move)
                        {
                            game_update_collisions();
                        }
                    }
#endif
                }
            }

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
        if (_game_state.player_fov[i])
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
    }

    // Render enemies
    // --------------
    for (size_t i = 0; i < _game_state.enemy_num; i++)
    {
        if (_game_state.enemies[i].alive)
        {
            if (_game_state.player_fov[util_p_to_i(_game_state.enemies[i].pos, SCREEN_COLS)])
            {
                render_render_tile(
                    (vec2) {
                        _game_state.enemies[i].pos.x * SCREEN_TILE_WIDTH,
                        _game_state.enemies[i].pos.y * SCREEN_TILE_WIDTH
                    },
                    _game_state.enemies[i].glyph,
                    _game_state.enemies[i].fg, _game_state.enemies[i].bg
                );
            }
        }
    }

    // Render player
    // -------------
    if (_game_state.player.alive)
    {
        render_render_tile(
            (vec2) {
                _game_state.player.pos.x * SCREEN_TILE_WIDTH,
                _game_state.player.pos.y * SCREEN_TILE_WIDTH
            },
            _game_state.player.glyph,
            _game_state.player.fg, _game_state.player.bg
        );
    }
}
