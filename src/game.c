#include "main.h"

typedef enum RunState
{
    INIT,
    AWAITING_INPUT,
    COMP_TURN,
} RunState;

typedef struct GameState
{
    Entity ent[ENTITY_NUM];
    Stats stats[ENTITY_NUM];
    size_t ent_by_pos[SCREEN_COLS * SCREEN_ROWS];
    bool collisions[SCREEN_COLS * SCREEN_ROWS];
    bool player_fov[SCREEN_COLS * SCREEN_ROWS];
    bool player_map_mem[SCREEN_COLS * SCREEN_ROWS];
    Map map;
    RunState run_state;
} GameState;

global_variable GameState _gs;

bool game_try_move_entity_p(Point *pos, Point new, int map_width)
{
    bool did_move = false;
    if (!_gs.collisions[util_p_to_i(new, map_width)])
    {
        *pos = new;
        did_move = true;
    }
    else if (util_p_cmp(_gs.ent[1].pos, new))
    {
        // printf("Attacking player.\n");

        // combat_attack(&_gs.enemy_stats[util_p_to_i(*pos, map_width)], &_gs.player_stats);

        did_move = true;
    }

    return did_move;
}

void game_update_collisions()
{
    memcpy(
        &_gs.collisions,
        &_gs.map.blocked,
        SCREEN_COLS * SCREEN_ROWS * sizeof(bool)
    );

    memset(_gs.ent_by_pos, 0, SCREEN_COLS * SCREEN_ROWS * sizeof(size_t));

    for (size_t i = 1; i < entity_get_count(); i++)
    {
        Entity e = _gs.ent[i];
        if ( _gs.ent[i].alive)
        {
            _gs.collisions[util_p_to_i(e.pos, SCREEN_COLS)] = true;
            _gs.ent_by_pos[util_p_to_i(e.pos, SCREEN_COLS)] = e.id;
        }
    }
}

void game_update(float dt, int *_new_key)
{
    switch (_gs.run_state)
    {
        case INIT:
        {
            map_gen_level(&_gs.map, SCREEN_COLS, SCREEN_ROWS);

            // Init player
            // -----------
            {
                Entity p = entity_ctor(util_xy_to_p(3, 30),
                                    GLM_VEC3_ZERO, (vec3) {1.0f, 1.0f, 0.5f },
                                    0x02, true);
                _gs.ent[p.id] = p;
                _gs.stats[p.id] = combat_stats_ctor("Player", 100, 100, 5, 5, 50, 30, 1);

                game_update_collisions();
                entity_calc_player_fov(
                    _gs.map.opaque, SCREEN_COLS, SCREEN_COLS,
                    _gs.ent[1].pos, _gs.player_fov, _gs.player_map_mem
                );
            }

            // Init enemies
            // ------------
            for (size_t i = 0; i < 30; i++)
            {
                Point pos;

                bool found = false;
                int attempts = 20;
                do
                {
                    pos.x = rand() % SCREEN_COLS;
                    pos.y = rand() % SCREEN_ROWS;
                    found = !_gs.collisions[util_p_to_i(pos, SCREEN_COLS)];
                    attempts--;
                }
                while (!found && attempts >= 0);

                if (found)
                {
                    Entity e = entity_ctor(pos, (vec3) { 0.5f, 0.15f, 0.15f }, (vec3) { 1.0f, 0.5f, 0.05f }, 'r', true);
                    _gs.ent[e.id] = e;
                    _gs.stats[e.id] = combat_stats_ctor("Rat", 100, 100, 5, 5, 50, 30, 1);

                    game_update_collisions();
                }
            }

            _gs.run_state = AWAITING_INPUT;
        } break;

        case AWAITING_INPUT:
        {
            bool skip_turn = false;

            Point move_to = { -1, -1 };

            if (*_new_key != GLFW_KEY_UNKNOWN)
            {
                switch (*_new_key)
                {
                    case GLFW_KEY_H:
                    {
                        move_to = util_xy_to_p(_gs.ent[1].pos.x - 1,
                                               _gs.ent[1].pos.y);
                    } break;

                    case GLFW_KEY_J:
                    {
                        move_to = util_xy_to_p(_gs.ent[1].pos.x,
                                               _gs.ent[1].pos.y + 1);
                    } break;

                    case GLFW_KEY_K:
                    {
                        move_to = util_xy_to_p(_gs.ent[1].pos.x,
                                               _gs.ent[1].pos.y - 1);
                    } break;

                    case GLFW_KEY_L:
                    {
                        move_to = util_xy_to_p(_gs.ent[1].pos.x + 1,
                                               _gs.ent[1].pos.y);
                    } break;

                    case GLFW_KEY_COMMA:
                    {
                        Point p;

                        int attempts = 20;
                        bool found = true;
                        do
                        {
                            p = util_xy_to_p(rand() % SCREEN_COLS, rand() % SCREEN_ROWS);
                            found = !_gs.collisions[util_p_to_i(p, SCREEN_COLS)];
                            attempts--;
                        }
                        while (!found && attempts >= 0);

                        if (found)
                        {
                            move_to = p;
                        }
                    } break;

                    case GLFW_KEY_PERIOD:
                    {
                        skip_turn = true;
                    } break;
                }

                *_new_key = GLFW_KEY_UNKNOWN;
            }

            bool did_move = false;

            if (move_to.x != -1 && move_to.y != -1)
            {
                did_move = game_try_move_entity_p(&_gs.ent[1].pos, move_to, SCREEN_COLS);
            }

            if (did_move || skip_turn)
            {
                entity_calc_player_fov(
                    _gs.map.opaque, SCREEN_COLS, SCREEN_ROWS,
                    _gs.ent[1].pos, _gs.player_fov, _gs.player_map_mem
                );
                game_update_collisions();
                _gs.run_state = COMP_TURN;
            }
        } break;

        case COMP_TURN:
        {
            for (size_t i = 1; i < entity_get_count(); i++)
            {
                if (_gs.ent[i].alive)
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
                                &_gs.ent[i].x,
                                &_gs.ent[i].y,
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
                            _gs.ent[i].pos.x,
                            _gs.ent[i].pos.y,
                            &new_x, &new_y,
                            (Direction) dir_iter,
                            SCREEN_COLS
                        );

                        int dist = util_calc_sqr_distance(
                            new_x, new_y,
                            _gs.ent[1].pos.x, _gs.ent[1].pos.y
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
                            &_gs.ent[i].pos.x,
                            &_gs.ent[i].pos.y,
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
                        _gs.map.opaque,
                        SCREEN_COLS, SCREEN_ROWS,
                        _gs.ent[i].pos,
                        _gs.ent[1].pos
                    ))
                    {
                        Point next = pathfinding_bfs(
                            _gs.collisions, SCREEN_COLS, SCREEN_ROWS,
                            _gs.ent[i].pos, _gs.ent[1].pos);

                        bool did_move = false;

                        if (next.x != -1 && next.y != -1)
                        {
                            did_move = game_try_move_entity_p(
                                &_gs.ent[i].pos,
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

            _gs.run_state = AWAITING_INPUT;
        } break;
    }
}

void game_render(float dt)
{
    // Render map
    // ----------
    for (size_t i = 0; i < SCREEN_COLS * SCREEN_ROWS; i++)
    {
        if (_gs.player_map_mem[i] || _gs.player_fov[i])
        {
            vec2 screen_offset ={
                (i % SCREEN_COLS) * SCREEN_TILE_WIDTH,
                (i / SCREEN_COLS) * SCREEN_TILE_WIDTH
            };

            render_render_tile(
                screen_offset,
                _gs.map.glyphs[i],
                _gs.map.fg_col[i], _gs.map.bg_col[i],
                !_gs.player_fov[i] && _gs.player_map_mem[i]
            );
        }
    }

    // Render entities
    // --------------
    for (size_t i = 1; i < entity_get_count(); i++)
    {
        if (_gs.ent[i].alive)
        {
            if (_gs.player_fov[util_p_to_i(_gs.ent[i].pos, SCREEN_COLS)])
            {
                render_render_tile(
                    (vec2) {
                        _gs.ent[i].pos.x * SCREEN_TILE_WIDTH,
                        _gs.ent[i].pos.y * SCREEN_TILE_WIDTH
                    },
                    _gs.ent[i].glyph,
                    _gs.ent[i].fg, _gs.ent[i].bg,
                    false
                );
            }
        }
    }
}
