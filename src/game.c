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

typedef enum PlayerState
{
    PSTATE_NONE,
    PSTATE_OVER_ITEM,
    PSTATE_OVER_EXIT
} PlayerState;

typedef struct GameState
{
    Entity ent[ENTITY_NUM];
    Stats stats[ENTITY_NUM];
    ItemType item_pickup[ENTITY_NUM];
    EntIdBag ent_by_pos[MAP_COLS * MAP_ROWS];
    bool collisions[MAP_COLS * MAP_ROWS];
    bool player_fov[MAP_COLS * MAP_ROWS];
    bool player_map_mem[MAP_COLS * MAP_ROWS];
    int player_items[ITEM_MAX];
    Map map;
    Glyph ui[UI_COLS * SCREEN_ROWS];
    char *log_lines[LOG_LINES];
    PlayerState player_state;
    RunState run_state;
    int current_level;
    int current_building;
    Point level_exit;
} GameState;

global_variable GameState _gs;
global_variable bool _skip_resetting_player_stats;
global_variable bool _skip_gen_new_building_number;

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
            _gs.player_state = PSTATE_OVER_ITEM;
            ui_draw_interact_item(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.item_pickup[target.ent_nc]);
        }
        else if(entity_id == 1 && _gs.player_state == PSTATE_OVER_ITEM)
        {
            _gs.player_state = PSTATE_NONE;
            ui_clean_interact(_gs.ui, UI_COLS, SCREEN_ROWS);
        }

        if (entity_id == 1 && util_p_cmp(_gs.level_exit, new))
        {
            _gs.player_state = PSTATE_OVER_EXIT;
            ui_draw_interact_exit(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.current_level + 1);
        }
        else if(entity_id == 1 && _gs.player_state == PSTATE_OVER_EXIT)
        {
            _gs.player_state = PSTATE_NONE;
            ui_clean_interact(_gs.ui, UI_COLS, SCREEN_ROWS);
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

    memset(_gs.ent_by_pos, 0, MAP_COLS * MAP_ROWS * sizeof(EntIdBag));

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

void game_pickup_item(ItemType item_type, Stats *player_stats)
{
   switch (item_type)
    {
        case ITEM_HEALTH:
        {
            int prev_health = player_stats->health;
            player_stats->health += 25;
            if (player_stats->health > player_stats->max_health)
            {
                player_stats->health = player_stats->max_health;
            }

            ui_add_log_line(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.log_lines,
                            "+ %d health.", player_stats->health - prev_health);
        } break;

        case ITEM_ARMOR_LEATHER:
        {
            player_stats->armor = ARMOR_LEATHER;
        } break;

        case ITEM_ARMOR_METAL:
        {
            player_stats->armor = ARMOR_METAL;
        } break;

        case ITEM_ARMOR_COMBAT:
        {
            player_stats->armor = ARMOR_COMBAT;
        } break;

        case ITEM_GUN_PISTOL:
        {
            player_stats->gun = GUN_PISTOL;
        } break;

        case ITEM_GUN_RIFLE:
        {
            player_stats->gun = GUN_RIFLE;
        } break;

        case ITEM_GUN_ROCKET:
        {
            player_stats->gun = GUN_ROCKET;
        } break;

        case ITEM_MECH_COMP:
        case ITEM_ELEC_COMP:
        case ITEM_JUNK:
        case ITEM_CPU_AUTOMAT_FRAME:
        case ITEM_MOBO_AUTOMAT_FRAME:
        case ITEM_GPU_AUTOMAT_FRAME:
        case ITEM_MEM_AUTOMAT_FRAME:
        case ITEM_ASSEMBLER_FRAME:
        {
            _gs.player_items[item_type] += 1;
            ui_draw_player_items(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.player_items);
        } break;

        default:
        {
        } break;
    }
}

void game_spawn_item(Point pos, ItemType item_type)
{
    switch (item_type)
    {
        case ITEM_HEALTH:
        {
            Entity e = entity_nc_ctor(pos,
                                      (vec3) { 0.1f, 0.6f, 0.1f },
                                      GLM_VEC3_ONE,
                                      '+',
                                      true);
            _gs.ent[e.id] = e;
            _gs.item_pickup[e.id] = ITEM_HEALTH;
        } break;

        case ITEM_ARMOR_LEATHER:
        {
            Entity e = entity_nc_ctor(pos,
                                      (vec3) { 0.2f, 0.2f, 0.2f },
                                      (vec3) { 0.3f, 0.1f, 0.1f },
                                      'y',
                                      true);
            _gs.ent[e.id] = e;
            _gs.item_pickup[e.id] = ITEM_ARMOR_LEATHER;
        } break;

        case ITEM_ARMOR_METAL:
        {
            Entity e = entity_nc_ctor(pos,
                                      (vec3) { 0.4f, 0.4f, 0.4f },
                                      (vec3) { 0.6f, 0.1f, 0.1f },
                                      'Y',
                                      true);
            _gs.ent[e.id] = e;
            _gs.item_pickup[e.id] = ITEM_ARMOR_METAL;
        } break;

        case ITEM_ARMOR_COMBAT:
        {
            Entity e = entity_nc_ctor(pos,
                                      (vec3) { 0.6f, 0.6f, 0.6f },
                                      (vec3) { 0.9f, 0.1f, 0.1f },
                                      0x9D,
                                      true);
            _gs.ent[e.id] = e;
            _gs.item_pickup[e.id] = ITEM_ARMOR_COMBAT;
        } break;

        case ITEM_GUN_PISTOL:
        {
            Entity e = entity_nc_ctor(pos,
                                      (vec3) { 0.2f, 0.2f, 0.2f },
                                      (vec3) { 0.3f, 0.1f, 0.1f },
                                      0xA9,
                                      true);
            _gs.ent[e.id] = e;
            _gs.item_pickup[e.id] = ITEM_GUN_PISTOL;
        } break;

        case ITEM_GUN_RIFLE:
        {
            Entity e = entity_nc_ctor(pos,
                                      (vec3) { 0.4f, 0.4f, 0.4f },
                                      (vec3) { 0.6f, 0.1f, 0.1f },
                                      0xF4,
                                      true);
            _gs.ent[e.id] = e;
            _gs.item_pickup[e.id] = ITEM_GUN_RIFLE;
        } break;

        case ITEM_GUN_ROCKET:
        {
            Entity e = entity_nc_ctor(pos,
                                      (vec3) { 0.6f, 0.6f, 0.6f },
                                      (vec3) { 0.9f, 0.1f, 0.1f },
                                      0x17,
                                      true);
            _gs.ent[e.id] = e;
            _gs.item_pickup[e.id] = ITEM_GUN_ROCKET;
        } break;

        case ITEM_MECH_COMP:
        {
            Entity e = entity_nc_ctor(pos,
                                      (vec3) { 0.6f, 0.2f, 0.6f },
                                      (vec3) { 0.3f, 0.1f, 0.1f },
                                      0xF0,
                                      true);
            _gs.ent[e.id] = e;
            _gs.item_pickup[e.id] = ITEM_MECH_COMP;
        } break;

        case ITEM_ELEC_COMP:
        {
            Entity e = entity_nc_ctor(pos,
                                      (vec3) { 0.6f, 0.2f, 0.6f },
                                      (vec3) { 0.3f, 0.1f, 0.1f },
                                      0xF7,
                                      true);
            _gs.ent[e.id] = e;
            _gs.item_pickup[e.id] = ITEM_ELEC_COMP;
        } break;

        case ITEM_JUNK:
        {
            Entity e = entity_nc_ctor(pos,
                                      (vec3) { 0.6f, 0.2f, 0.6f },
                                      (vec3) { 0.3f, 0.1f, 0.1f },
                                      '&',
                                      true);
            _gs.ent[e.id] = e;
            _gs.item_pickup[e.id] = ITEM_JUNK;
        } break;

        case ITEM_CPU_AUTOMAT_FRAME:
        {
            Entity e = entity_nc_ctor(pos,
                                        (vec3) { 0.9f, 0.5f, 0.9f },
                                        (vec3) { 1.0f, 0.3f, 0.3f },
                                        0x15,
                                        true);
            _gs.ent[e.id] = e;
            _gs.item_pickup[e.id] = ITEM_CPU_AUTOMAT_FRAME;
        } break;

        case ITEM_MOBO_AUTOMAT_FRAME:
        {
            Entity e = entity_nc_ctor(pos,
                                      (vec3) { 0.9f, 0.5f, 0.9f },
                                      (vec3) { 1.0f, 0.3f, 1.0f },
                                      0x15,
                                      true);
            _gs.ent[e.id] = e;
            _gs.item_pickup[e.id] = ITEM_MOBO_AUTOMAT_FRAME;
        } break;

        case ITEM_GPU_AUTOMAT_FRAME:
        {
            Entity e = entity_nc_ctor(pos,
                                      (vec3) { 0.9f, 0.5f, 0.9f },
                                      (vec3) { 0.3f, 0.3f, 1.0f },
                                      0x15,
                                      true);
            _gs.ent[e.id] = e;
            _gs.item_pickup[e.id] = ITEM_GPU_AUTOMAT_FRAME;
        } break;

        case ITEM_MEM_AUTOMAT_FRAME:
        {
            Entity e = entity_nc_ctor(pos,
                                      (vec3) { 0.9f, 0.5f, 0.9f },
                                      (vec3) { 0.3f, 1.0f, 1.0f },
                                      0x15,
                                      true);
            _gs.ent[e.id] = e;
            _gs.item_pickup[e.id] = ITEM_MEM_AUTOMAT_FRAME;
        } break;

        case ITEM_ASSEMBLER_FRAME:
        {
                Entity e = entity_nc_ctor(pos,
                                          (vec3) { 0.9f, 0.5f, 0.9f },
                                          (vec3) { 0.3f, 1.0f, 0.3f },
                                          0x15,
                                          true);
                _gs.ent[e.id] = e;
                _gs.item_pickup[e.id] = ITEM_ASSEMBLER_FRAME;
        } break;

        default:
        {
        } break;
    }
}


void game_spawn_item_randomly(ItemType item_type)
{
    Point pos;

    bool found = false;
    int attempts = 20;
    do
    {
        pos.x = rand() % MAP_COLS;
        pos.y = rand() % MAP_ROWS;
        found = !_gs.collisions[util_p_to_i(pos, MAP_COLS)];
        found &= !_gs.ent_by_pos[util_p_to_i(pos, MAP_COLS)].ent_nc;
        found &= !util_p_cmp(pos, _gs.level_exit); // TODO: Didn't test this.
        attempts--;
    }
    while (!found && attempts >= 0);

    game_spawn_item(pos, item_type);
}

void game_spawn_level_items()
{
    size_t num[ITEM_MAX] = {0};

    switch (_gs.current_level)
    {
        default:
        {
            num[ITEM_HEALTH]             = 10;

            num[ITEM_ARMOR_LEATHER]      = 1;
            num[ITEM_ARMOR_METAL]        = 1;
            num[ITEM_ARMOR_COMBAT]       = 1;

            num[ITEM_GUN_PISTOL]         = 1;
            num[ITEM_GUN_RIFLE]          = 1;
            num[ITEM_GUN_ROCKET]         = 1;

            num[ITEM_MECH_COMP]          = 1;
            num[ITEM_ELEC_COMP]          = 1;
            num[ITEM_JUNK]               = 1;

            num[ITEM_CPU_AUTOMAT_FRAME]  = 1;
            num[ITEM_MOBO_AUTOMAT_FRAME] = 1;
            num[ITEM_GPU_AUTOMAT_FRAME]  = 1;
            num[ITEM_MEM_AUTOMAT_FRAME]  = 1;
            num[ITEM_ASSEMBLER_FRAME]    = 1;
        } break;

        // default:
        // {

        // } break;
    }

    for (int type = ITEM_HEALTH; type < ITEM_MAX; type++)
    {
        for (size_t i = 0; i < num[type]; i++)
        {
            game_spawn_item_randomly(type);
            game_update_collisions();
        }
    }
}

void game_spawn_enemy(Point pos, EnemyType enemy_type)
{
    switch (enemy_type)
    {
        case ENEMY_RAT:
        {
            Entity e = entity_char_ctor(pos,
                                        (vec3) { 0.3f, 0.15f, 0.15f },
                                        (vec3) { 1.0f, 0.3f, 0.05f },
                                        'r',
                                        true);
            _gs.ent[e.id] = e;
            _gs.stats[e.id] = combat_stats_ctor("Rat",
                                                30,
                                                9, 4,
                                                7, 2,
                                                1,
                                                ARMOR_NONE, GUN_NONE);
        } break;

        case ENEMY_ZOMBIE:
        {
            Entity e = entity_char_ctor(pos,
                                        (vec3) { 0.5f, 0.15f, 0.15f },
                                        (vec3) { 1.0f, 0.5f, 0.05f },
                                        'z',
                                        true);
            _gs.ent[e.id] = e;
            _gs.stats[e.id] = combat_stats_ctor("Zombie",
                                                50,
                                                3, 3,
                                                20, 2,
                                                1,
                                                ARMOR_NONE, GUN_NONE);
        } break;

        case ENEMY_SAVAGE:
        {
            Entity e = entity_char_ctor(pos,
                                        (vec3) { 0.7f, 0.15f, 0.15f },
                                        (vec3) { 1.0f, 0.7f, 0.05f },
                                        'S',
                                        true);
            _gs.ent[e.id] = e;
            _gs.stats[e.id] = combat_stats_ctor("Savage",
                                                80,
                                                7, 6,
                                                21, 20,
                                                1,
                                                ARMOR_NONE, GUN_NONE);
        } break;

        case ENEMY_ROBOT:
        {
            Entity e = entity_char_ctor(pos,
                                        (vec3) { 1.0f, 0.15f, 0.15f },
                                        (vec3) { 1.0f, 1.0f, 0.05f },
                                        'R',
                                        true);
            _gs.ent[e.id] = e;
            _gs.stats[e.id] = combat_stats_ctor("Robot",
                                                150,
                                                9, 2,
                                                25, 39,
                                                1,
                                                ARMOR_NONE, GUN_NONE);
        } break;

        default:
        {

        } break;
    }
}

// void game_spawn_enemies_randomly(int count, Glyph glyph, )
// {
//     for (size_t i = 0; i < 30; i++)
//     {
//         Point pos;

//         bool found = false;
//         int attempts = 20;
//         do
//         {
//             pos.x = rand() % MAP_COLS;
//             pos.y = rand() % MAP_ROWS;
//             found = !_gs.collisions[util_p_to_i(pos, MAP_COLS)];
//             found &= !util_p_cmp(pos, _gs.level_exit); // TODO: Didn't test this.
//             attempts--;
//         }
//         while (!found && attempts >= 0);

//         if (found)
//         {
//             game_update_collisions();
//         }
//     }
// }

void game_spawn_player(Point pos)
{
    Entity p = entity_char_ctor(pos,
                                GLM_VEC3_ZERO,
                                (vec3) {1.0f, 1.0f, 0.5f },
                                0x02, true);
    _gs.ent[p.id] = p;

    if (!_skip_resetting_player_stats)
    {
        _gs.stats[p.id] = combat_stats_ctor("Player", 
                                            100,
                                            7, 7,
                                            20, 2,
                                            1,
                                            ARMOR_NONE, GUN_NONE);

        _skip_resetting_player_stats = true;
    }

    game_update_collisions();
    entity_calc_player_fov(
        _gs.map.opaque, MAP_COLS, MAP_COLS,
        _gs.ent[1].pos, _gs.player_fov, _gs.player_map_mem
    );
}

void game_spawn_exit()
{
    for (size_t i = 0; i < 1; i++)
    {
        Point pos;

        bool found = false;
        int attempts = 10000;
        do
        {
            pos.x = rand() % MAP_COLS;
            pos.y = rand() % MAP_ROWS;
            found = !_gs.map.blocked[util_p_to_i(pos, MAP_COLS)];
            attempts--;
        }
        while (!found && attempts >= 0);

        if (!found)
        {
            printf("game_spawn_exit: Could not find a place for exit after 10,000 attempts. Setting at 1, 1.\n");
            pos = util_xy_to_p(1, 1);
        }

        printf("level exit: %d, %d\n", pos.x, pos.y);

        _gs.level_exit = pos;
        _gs.map.blocked[util_p_to_i(pos, MAP_COLS)] = false; // Just in case exit was spawned at a
        _gs.map.opaque[util_p_to_i(pos, MAP_COLS)] = false;  // forced location, make it walkable

        game_update_collisions();
    }
}

void game_clean()
{
    // Reset entities
    // -------------
    memset(_gs.ent, 0, ENTITY_NUM * sizeof(Entity));
    entity_reset();

    // Reset entity stats
    // ------------------
    memset(_gs.stats + 2, 0, (ENTITY_NUM - 2) * sizeof(Entity));

    // Reset item pickups
    // ------------------
    memset(_gs.item_pickup, 0, ENTITY_NUM * sizeof(ItemType));

    // Reset entity pos references
    // ---------------------------
    memset(_gs.ent_by_pos, 0, MAP_COLS * MAP_ROWS * sizeof(EntIdBag));

    // Reset collisions
    // ----------------
    memset(_gs.collisions, 0, MAP_COLS * MAP_ROWS * sizeof(bool));

    // Reset player fov
    // ----------------
    memset(_gs.player_fov, 0, MAP_COLS * MAP_ROWS * sizeof(bool));

    // Reset player map memory
    // ----------------
    memset(_gs.player_map_mem, 0, MAP_COLS * MAP_ROWS * sizeof(bool));

    // Reset map
    // ---------
    memset(&_gs.map, 0, sizeof(Map));

    // Reset UI
    // --------
    memset(_gs.ui, 0, UI_COLS * SCREEN_ROWS * sizeof(Glyph));

    // Reset player_over_item
    // ----------------------
    _gs.player_state = PSTATE_NONE;

    // Reset log lines
    // ---------------
    for (int i = 0; i < LOG_LINES; i++)
    {
        free(_gs.log_lines[i]);
        _gs.log_lines[i] = NULL;
    }
    ui_reset_log_cursor();
}

void game_update(float dt, int *_new_key)
{
    switch (_gs.run_state)
    {
        case INIT:
        {
            printf("Loading %d level...\n", _gs.current_level);

            game_clean();

            map_gen_level(&_gs.map, MAP_COLS, MAP_ROWS);

            // Randomize exit
            // --------------
            game_spawn_exit();

            // Init player
            // -----------
            Point player_pos = { 3, 30 };
            game_spawn_player(player_pos);

            // Init enemies
            // ------------
            // game_spawn_level_enemies();
            Point rat_pos = { 5, 30 };
            // game_spawn_enemy(rat_pos, ENEMY_RAT);

            Point zombie_pos = { 5, 30 };
            // game_spawn_enemy(zombie_pos, ENEMY_ZOMBIE);

            Point savage_pos = { 5, 30 };
            // game_spawn_enemy(savage_pos, ENEMY_SAVAGE);

            Point robot_pos = { 5, 30 };
            game_spawn_enemy(robot_pos, ENEMY_ROBOT);

            // Init items
            // ----------
            // game_spawn_level_items();

            // Init UI
            // -------
            ui_draw_header(_gs.ui, UI_COLS, SCREEN_ROWS);

            if (!_skip_gen_new_building_number)
            {
                _gs.current_building = rand() % 10000;
                _skip_gen_new_building_number = true;
            }
            ui_draw_location(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.current_level + 1, _gs.current_building);

            for (int i = 0; i < LOG_LINES; i++)
            {
                _gs.log_lines[i] = calloc(1, UI_COLS * sizeof(char));
            }

            ui_draw_log(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.log_lines);

            ui_add_log_line(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.log_lines, 
                            "Hello, player %c! hjkl-move .-skip", 0x01);

            ui_draw_player_stats(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.stats[1]);

            ui_draw_player_items(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.player_items);

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
                        switch (_gs.player_state)
                        {
                            case PSTATE_OVER_ITEM:
                            {
                                size_t e_id = 
                                    _gs.ent_by_pos[util_p_to_i(_gs.ent[1].pos, MAP_COLS)].ent_nc;

                                _gs.ent[e_id].alive = false;

                                ItemType item_type = _gs.item_pickup[e_id];
                                AString item_name = item_get_item_name(item_type);
                                ui_add_log_line(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.log_lines,
                                                "%s picked up.", item_name.str);

                                game_pickup_item(item_type, &_gs.stats[1]);

                                ui_draw_player_stats(_gs.ui, UI_COLS, SCREEN_ROWS, _gs.stats[1]);

                                ui_clean_interact(_gs.ui, UI_COLS, SCREEN_ROWS);

                                _gs.player_state = PSTATE_NONE;

                                skip_turn = true;
                            } break;

                            case PSTATE_OVER_EXIT:
                            {
                                _gs.current_level++;
                                _gs.run_state = INIT;
                            } break;

                            default:
                            {

                            } break;
                        }
                    } break;

                    case GLFW_KEY_BACKSPACE:
                    {
                        _gs.current_level++;
                        _gs.run_state = INIT;
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

    // Render exit
    // -----------
    if (_gs.player_map_mem[util_p_to_i(_gs.level_exit, MAP_ROWS)]
        || _gs.player_fov[util_p_to_i(_gs.level_exit, MAP_ROWS)])
    {
        render_render_tile((vec2) { _gs.level_exit.x * SCREEN_TILE_WIDTH,
                                    _gs.level_exit.y * SCREEN_TILE_WIDTH
                        },
                        0xAF,
                        GLM_VEC3_ONE,
                        (vec3) { 0.2f, 0.2f, 0.2f },
                        !_gs.player_fov[util_p_to_i(_gs.level_exit, MAP_ROWS)] 
                            && _gs.player_map_mem[util_p_to_i(_gs.level_exit, MAP_ROWS)]);
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
        else if (p.x == 0 && p.y == UI_ITEMS_ROW)
        {
            glyph = 0xC3;
        }
        else if (p.x == UI_COLS - 1 && p.y == UI_ITEMS_ROW)
        {
            glyph = 0xB4;
        }
        else if (p.x == 0 && p.y == UI_LOCATION_ROW)
        {
            glyph = 0xC3;
        }
        else if (p.x == UI_COLS - 1 && p.y == UI_LOCATION_ROW)
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
              || p.y == UI_ITEMS_ROW
              || p.y == UI_LOCATION_ROW
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
