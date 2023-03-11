#include "main.h"

typedef enum RunState
{
    INIT,
    AWAITING_INPUT,
    COMP_TURN,
    GAME_OVER
} RunState;

typedef struct EntIdBag
{
    size_t ent_char;
    size_t ent_nc;
} EntIdBag;

typedef struct GameState
{
    Entity ent[ENTITY_NUM];
    Stats stats[ENTITY_NUM];
    ItemType item_pickup[ENTITY_NUM];
    EntIdBag ent_by_pos[MAP_COLS * MAP_ROWS];
    bool collisions[MAP_COLS * MAP_ROWS];
    bool player_fov[MAP_COLS * MAP_ROWS];
    bool player_map_mem[MAP_COLS * MAP_ROWS];
    int player_items[ITEM_NONE];
    Map map;
    Glyph ui[UI_COLS * SCREEN_ROWS];
    char *log_lines[LOG_LINES]; // TODO: clean up if stop showing log at some point
    bool player_over_item;
    RunState run_state;
} GameState;

global_variable GameState _gs;

bool game_try_move_entity_p(size_t entity_id, Point *pos, Point new, int map_width)
{
    EntIdBag target = _gs.ent_by_pos[util_p_to_i(new, map_width)];

    bool did_move = false;
    if (!_gs.collisions[util_p_to_i(new, map_width)])
    {
        *pos = new;
        did_move = true;

        // Item pickup logic
        // If player stepped over an item pickup entity ...
        if (entity_id == 1 && target.ent_nc >= ENTITY_NC_OFFSET)
        {
            _gs.player_over_item = true;
            ui_draw_pickup_item(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.item_pickup[target.ent_nc]);
        }
        else if(entity_id == 1 && _gs.player_over_item)
        {
            _gs.player_over_item = false;
            ui_clean_pickup_item(_gs.ui, UI_COLS, SCREEN_ROWS);
        }

    }
    else if (target.ent_char)
    {
        Stats *att = &_gs.stats[entity_id];
        Stats *def = &_gs.stats[target.ent_char];
        AttackResult ar = combat_attack(att, def);

        ui_add_log_line(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.log_lines, 
                        "%s>%s - %s-%dhp.",
                        att->name, def->name, ar.hit_type, ar.end_dmg);

        if (def->health <= 0)
        {
            _gs.ent[target.ent_char].alive = false;
            ui_add_log_line(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.log_lines, 
                        "%s died.", def->name);
        }

        // If player was attacked, update ui with their stats
        if (target.ent_char == 1)
        {
            ui_draw_player_stats(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.stats[1]);
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

    for (size_t i = 0; i < entity_nc_get_count(); i++)
    {
        int offset_i = i + ENTITY_NC_OFFSET;

        Entity e = _gs.ent[offset_i];
        if (e.alive)
        {
            _gs.ent_by_pos[util_p_to_i(e.pos, MAP_COLS)].ent_nc = e.id;
        }
    }

    for (size_t i = 1; i < entity_char_get_count(); i++)
    {
        Entity e = _gs.ent[i];
        if (e.alive)
        {
            _gs.collisions[util_p_to_i(e.pos, MAP_COLS)] = true;
            _gs.ent_by_pos[util_p_to_i(e.pos, MAP_COLS)].ent_char = e.id;
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
                Entity p = entity_char_ctor(util_xy_to_p(3, 30),
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
                    Entity e = entity_char_ctor(pos, (vec3) { 0.5f, 0.15f, 0.15f }, (vec3) { 1.0f, 0.5f, 0.05f }, 'r', true);
                    _gs.ent[e.id] = e;
                    _gs.stats[e.id] = combat_stats_ctor("Rat", 30, 1, 5, 7, 2, 1);

                    game_update_collisions();
                }
            }

            // Init items
            // ----------
            for (size_t i = 0; i < 1; i++)
            {
                Entity e = entity_nc_ctor(util_xy_to_p(5, 30),
                                          (vec3) { 0.1f, 0.6f, 0.1f },
                                          GLM_VEC3_ONE,
                                          '+',
                                          true);
                _gs.ent[e.id] = e;
                _gs.item_pickup[e.id] = ITEM_HEALTH;

                game_update_collisions();
            }

            // Init UI
            // -------
            Point p = { 15, 2 };
            ui_printf(_gs.ui, UI_COLS, SCREEN_ROWS, p, "%c %c struo", 0xE0, 0xEA);

            for (int i = 0; i < LOG_LINES; i++)
            {
                _gs.log_lines[i] = calloc(1, UI_COLS * sizeof(char));
            }

            ui_draw_log(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.log_lines);

            ui_add_log_line(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.log_lines, 
                            "Hello, player %c! hjkl-move .-skip", 0x01);

            ui_draw_player_stats(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.stats[1]);

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

                    case GLFW_KEY_G:
                    {
                        if (_gs.player_over_item)
                        {
                            size_t e_id = 
                                _gs.ent_by_pos[util_p_to_i(_gs.ent[1].pos, MAP_COLS)].ent_nc;

                            _gs.ent[e_id].alive = false;
                            // ItemType item_type = _gs.item_pickup[e_id];

                            printf("Item %zu picked up.\n", e_id);

                            ui_clean_pickup_item(_gs.ui, UI_COLS, SCREEN_ROWS);

                            skip_turn = true;
                        }
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
            for (size_t i = 1; i < entity_char_get_count(); i++)
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

                ui_add_log_line(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.log_lines, "");
                ui_add_log_line(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.log_lines, "GAME OVER.");
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

    // Render non-char entities
    // --------------
    for (size_t i = 0; i < entity_nc_get_count(); i++)
    {
        size_t offset_i = i + ENTITY_NC_OFFSET;
        if (_gs.ent[offset_i].alive)
        {
            if (_gs.player_fov[util_p_to_i(_gs.ent[offset_i].pos, MAP_COLS)])
            {
                render_render_tile(
                    (vec2) {
                        _gs.ent[offset_i].pos.x * SCREEN_TILE_WIDTH,
                        _gs.ent[offset_i].pos.y * SCREEN_TILE_WIDTH
                    },
                    _gs.ent[offset_i].glyph,
                    _gs.ent[offset_i].fg, _gs.ent[offset_i].bg,
                    false
                );
            }
        }
    }

    // Render char entities
    // --------------
    for (size_t i = 2; i < entity_char_get_count(); i++)
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

    // Render player entity
    // --------------
        if (_gs.ent[1].alive)
        {
            render_render_tile(
                (vec2) {
                    _gs.ent[1].pos.x * SCREEN_TILE_WIDTH,
                    _gs.ent[1].pos.y * SCREEN_TILE_WIDTH
                },
                _gs.ent[1].glyph,
                _gs.ent[1].fg, _gs.ent[1].bg,
                false
            );
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
        else if (p.x == 0 && p.y == SCREEN_ROWS - LOG_LINES - 3)
        {
            glyph = 0xC3;
        }
        else if (p.x == UI_COLS - 1 && p.y == SCREEN_ROWS - LOG_LINES - 3)
        {
            glyph = 0xB4;
        }
        else if (p.x == 0 && p.y == UI_STATS_ROW)
        {
            glyph = 0xC3;
        }
        else if (p.x == UI_COLS - 1 && p.y == UI_STATS_ROW)
        {
            glyph = 0xB4;
        }
        else if (p.x == 0 || p.x == UI_COLS - 1)
        {
            glyph = 0xB3;
        }
        else if (p.y == 0 
              || p.y == SCREEN_ROWS - LOG_LINES - 3
              || p.y == UI_STATS_ROW
              || p.y == SCREEN_ROWS - 1)
        {
            glyph = 0xC4;
        }

        Glyph glyph_in_pos = _gs.ui[i];
        if (glyph_in_pos)
        {
            glyph = glyph_in_pos;
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
