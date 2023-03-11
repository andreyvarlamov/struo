#include "main.h"

typedef struct Entity
{
    size_t id;
    Point pos;
    vec3 bg;
    vec3 fg; 
    Glyph glyph;
    bool alive;
} Entity;

typedef enum EnemyType
{
    ENEMY_NONE,

    ENEMY_RAT,
    ENEMY_ZOMBIE,
    ENEMY_SAVAGE,
    ENEMY_ROBOT,

    ENEMY_MAX
} EnemyType;

typedef enum MachineType
{
    MACHINE_NONE,

    MACHINE_CPU_AUTOMATON,
    MACHINE_MOBO_AUTOMATON,
    MACHINE_GPU_AUTOMATON,
    MACHINE_MEM_AUTOMATON,
    MACHINE_ASSEMBLER,

    MACHINE_COMPUTER,

    MACHINE_MAX
} MachineType;

typedef struct MachineEntity
{
    Entity e_plan;
    Entity e_built;
    bool built;
} MachineEntity;

global_variable size_t next_char_id = 1;
global_variable size_t next_nc_id = ENTITY_NC_OFFSET;

void entity_reset()
{
    next_char_id = 1;
    next_nc_id = ENTITY_NC_OFFSET;
}

Entity entity_char_ctor(Point pos, vec3 bg, vec3 fg, Glyph glyph, bool alive)
{
    if (next_char_id >= ENTITY_NC_OFFSET)
    {
        printf("entity_char_ctor: ERROR: creating more entities than allowed");
    }

    Entity e;
    e.id = next_char_id;
    next_char_id++;

    e.pos = pos;
    glm_vec3_copy(bg, e.bg);
    glm_vec3_copy(fg, e.fg);
    e.glyph = glyph;
    e.alive = alive;

    return e;
}

Entity entity_nc_ctor(Point pos, vec3 bg, vec3 fg, Glyph glyph, bool alive)
{
    if (next_char_id >= ENTITY_NUM)
    {
        printf("entity_nc_ctor: ERROR: creating more entities than allowed");
    }

    Entity e;
    e.id = next_nc_id;
    next_nc_id++;

    e.pos = pos;
    glm_vec3_copy(bg, e.bg);
    glm_vec3_copy(fg, e.fg);
    e.glyph = glyph;
    e.alive = alive;

    return e;
}

size_t entity_char_get_count()
{
    return next_char_id;
}

size_t entity_nc_get_count()
{
    return next_nc_id - ENTITY_NC_OFFSET;
}

bool entity_check_pos_within_fov(const bool *opaque, int map_width, int map_height, 
    Point pos, Point goal)
{
    if (!util_check_p_within_rad(pos, ENEMY_FOV, goal))
    {
        return false;
    }

    Point d = util_xy_to_p(goal.x - pos.x, goal.y - pos.y);
    Point n = util_xy_to_p(abs(d.x), abs(d.y));
    Point sign = util_xy_to_p(
        d.x > 0 ? 1 : -1,
        d.y > 0 ? 1 : -1
    );

    bool *blocked = malloc(map_width * map_height * sizeof(bool));
    memcpy(blocked, opaque, map_width * map_height * sizeof(bool));
    blocked[util_p_to_i(goal, map_width)] = false; // HACKY

    bool is_blocked = false;

    for(Point i = { 0, 0 }; i.x < n.x || i.y < n.y;)
    {
        int decision = (1 + 2 * i.x) * n.y - (1 + 2 * i.y) * n.x;
        if (decision == 0)
        {
            // Diagonal step
            pos.x += sign.x;
            pos.y += sign.y;
            i.x++;
            i.y++;

        }
        else if (decision < 0)
        {
            // Horizontal step
            pos.x += sign.x;
            i.x++;
        }
        else
        {
            // Vertical step
            pos.y += sign.y;
            i.y++;
        }

        if (blocked[util_p_to_i(pos, map_width)])
        {
            is_blocked = true;
            break;
        }
    }

    free(blocked);
    return !is_blocked;
}

void entity_calc_player_fov(const bool *opaque, int map_width , int map_height,
    Point pos, bool *fov, bool *map_mem)
{
    for (int i = 0; i < map_width * map_height; i++)
    {
        fov[i] = false;
    }

    int top_b = pos.y - PLAYER_FOV + 1; top_b = top_b > 0 ? top_b : 0;
    int bot_b = pos.y + PLAYER_FOV; bot_b = bot_b < map_height ? bot_b : map_height;
    int lft_b = pos.x - PLAYER_FOV + 1; lft_b = lft_b > 0 ? lft_b : 0;
    int rgt_b = pos.x + PLAYER_FOV; rgt_b = rgt_b < map_width ? rgt_b : map_width;

    for (int y = top_b; y < bot_b; y++)
    {
        for (int x = lft_b; x < rgt_b; x++)
        {
            Point p = { x, y };
            if (util_check_p_within_rad(pos, PLAYER_FOV, p))
            {
                if (entity_check_pos_within_fov(
                    opaque, map_width, map_height,
                    pos, p
                ))
                {
                    fov[util_p_to_i(p, map_width)] = true;
                    map_mem[util_p_to_i(p, map_width)] = true;
                }
            }
        }
    } 
}

AString entity_get_machine_name(MachineType machine_type)
{
    AString name = {0};

    switch(machine_type)
    {
        case MACHINE_CPU_AUTOMATON:
        {
            strcpy(name.str, "CPU Automaton");
        } break;

        case MACHINE_MOBO_AUTOMATON:
        {
            strcpy(name.str, "Motherboard Automaton");
        } break;

        case MACHINE_GPU_AUTOMATON:
        {
            strcpy(name.str, "GPU Automaton");
        } break;

        case MACHINE_MEM_AUTOMATON:
        {
            strcpy(name.str, "Memory Automaton");
        } break;

        case MACHINE_ASSEMBLER:
        {
            strcpy(name.str, "Computer Assembler");
        } break;

        case MACHINE_COMPUTER:
        {
            strcpy(name.str, "Computer");
        } break;

        default:
        {

        } break;
    }

    return name;
}
