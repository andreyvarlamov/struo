#include "main.h"

typedef enum RunState
{
    INIT,
    AWAITING_INPUT,
    COMP_TURN,
    GAME_OVER
} RunState;

typedef struct GameState
{
    Entity ent[ENTITY_NUM];
    Stats stats[ENTITY_NUM];
    size_t ent_by_pos[MAP_COLS * MAP_ROWS];
    bool collisions[MAP_COLS * MAP_ROWS];
    bool player_fov[MAP_COLS * MAP_ROWS];
    bool player_map_mem[MAP_COLS * MAP_ROWS];
    Map map;
    RunState run_state;
} GameState;

global_variable GameState _gs;

bool game_try_move_entity_p(size_t entity_id, Point *pos, Point new, int map_width)
{
    size_t target_id = _gs.ent_by_pos[util_p_to_i(new, map_width)];

    bool did_move = false;
    if (!_gs.collisions[util_p_to_i(new, map_width)])
    {
        *pos = new;
        did_move = true;
    }
    else if (target_id)
    {
        Stats *att = &_gs.stats[entity_id];
        Stats *def = &_gs.stats[target_id];
        combat_attack(att, def);

        if (def->health <= 0)
        {
            _gs.ent[target_id].alive = false;
            printf("%s died.\n", def->name);
        }

        did_move = true;
    }

    return did_move;
}

void game_update_collisions()
{
    memcpy(
        &_gs.collisions,
        &_gs.map.blocked,
        MAP_COLS * MAP_ROWS * sizeof(bool)
    );

    memset(_gs.ent_by_pos, 0, MAP_COLS * MAP_ROWS * sizeof(size_t));

    for (size_t i = 1; i < entity_get_count(); i++)
    {
        Entity e = _gs.ent[i];
        if ( _gs.ent[i].alive)
        {
            _gs.collisions[util_p_to_i(e.pos, MAP_COLS)] = true;
            _gs.ent_by_pos[util_p_to_i(e.pos, MAP_COLS)] = e.id;
        }
    }
}

void game_update(float dt, int *_new_key)
{
    switch (_gs.run_state)
    {
        case INIT:
        {
            map_gen_level(&_gs.map, MAP_COLS, MAP_ROWS);

            // Init player
            // -----------
            {
                Entity p = entity_ctor(util_xy_to_p(3, 30),
                                    GLM_VEC3_ZERO, (vec3) {1.0f, 1.0f, 0.5f },
                                    0x02, true);
                _gs.ent[p.id] = p;
                _gs.stats[p.id] = combat_stats_ctor("Player", 100, 5, 5, 20, 2, 1);

                game_update_collisions();
                entity_calc_player_fov(
                    _gs.map.opaque, MAP_COLS, MAP_COLS,
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
                    pos.x = rand() % MAP_COLS;
                    pos.y = rand() % MAP_ROWS;
                    found = !_gs.collisions[util_p_to_i(pos, MAP_COLS)];
                    attempts--;
                }
                while (!found && attempts >= 0);

                if (found)
                {
                    Entity e = entity_ctor(pos, (vec3) { 0.5f, 0.15f, 0.15f }, (vec3) { 1.0f, 0.5f, 0.05f }, 'r', true);
                    _gs.ent[e.id] = e;
                    _gs.stats[e.id] = combat_stats_ctor("Rat", 30, 1, 5, 7, 2, 1);

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
                            p = util_xy_to_p(rand() % MAP_COLS, rand() % MAP_ROWS);
                            found = !_gs.collisions[util_p_to_i(p, MAP_COLS)];
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
                did_move = game_try_move_entity_p(1, &_gs.ent[1].pos, move_to, MAP_COLS);
            }

            if (did_move || skip_turn)
            {
                entity_calc_player_fov(
                    _gs.map.opaque, MAP_COLS, MAP_ROWS,
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
                                MAP_COLS
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
                            MAP_COLS
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
                            MAP_COLS
                        );

                        if (did_move)
                        {
                            game_update_collisions();
                        }
                    }
#elif 1
                    if (entity_check_pos_within_fov(
                        _gs.map.opaque,
                        MAP_COLS, MAP_ROWS,
                        _gs.ent[i].pos,
                        _gs.ent[1].pos
                    ))
                    {
                        Point next = pathfinding_bfs(
                            _gs.collisions, MAP_COLS, MAP_ROWS,
                            _gs.ent[i].pos, _gs.ent[1].pos);

                        bool did_move = false;

                        if (next.x != -1 && next.y != -1)
                        {
                            did_move = game_try_move_entity_p(
                                i,
                                &_gs.ent[i].pos,
                                next,
                                MAP_COLS
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

            if (!_gs.ent[1].alive)
            {
                _gs.run_state = GAME_OVER;
                printf("GAME OVER\n");
            }
            else
            {
                _gs.run_state = AWAITING_INPUT;
            }

        } break;

        case GAME_OVER:
        {

        } break;
    }
}

void game_render(float dt)
{
    // Render map
    // ----------
    for (size_t i = 0; i < MAP_COLS * MAP_ROWS; i++)
    {
        if (_gs.player_map_mem[i] || _gs.player_fov[i])
        {
            vec2 screen_offset ={
                (i % MAP_COLS) * SCREEN_TILE_WIDTH,
                (i / MAP_COLS) * SCREEN_TILE_WIDTH
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
            if (_gs.player_fov[util_p_to_i(_gs.ent[i].pos, MAP_COLS)])
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

    // Render UI
    // ---------
    for (int i = 0; i <  UI_COLS * SCREEN_ROWS; i++)
    {
        Point p = util_i_to_p(i, UI_COLS);
        Glyph glyph = ' ';
        
        if (p.x == 0 && p.y == 0)
        {
            glyph = 0xDA;
        }
        else if (p.x == UI_COLS - 1 && p.y == 0)
        {
            glyph = 0xBF;
        }
        else if (p.x == UI_COLS - 1 && p.y == SCREEN_ROWS - 1)
        {
            glyph = 0xD9;
        }
        else if (p.x == 0 && p.y == SCREEN_ROWS - 1)
        {
            glyph = 0xC0;
        }
        else if (p.x == 0 || p.x == UI_COLS - 1)
        {
            glyph = 0xB3;
        }
        else if (p.y == 0 || p.y == SCREEN_ROWS - 1)
        {
            glyph = 0xC4;
        }
        render_render_tile(
                    (vec2) {
                        (p.x + MAP_COLS) * SCREEN_TILE_WIDTH,
                        p.y * SCREEN_TILE_WIDTH
                    },
                    glyph,
                    GLM_VEC3_ONE, GLM_VEC3_ZERO,
                    false
        );
    }
}
