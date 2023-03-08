#include "main.h"

typedef struct Map
{
    vec3 fg_col[SCREEN_COLS * SCREEN_ROWS];
    vec3 bg_col[SCREEN_COLS * SCREEN_ROWS];
    Glyph glyphs[SCREEN_COLS * SCREEN_ROWS];
    bool blocked[SCREEN_COLS * SCREEN_ROWS];
} Map;

// to_fill should come preallocated and initialized
void _map_gen_room(
    char *to_fill, int map_width, int map_height,
    Rect room)
{
    if (!util_check_rect_in_bounds(room, map_width, map_height))
    {
        printf("_map_gen_rooms: WARNING: Room out of bounds. Skipping...\n");
        return;
    }

    // Draw horizontal boundaries
    for (int i = room.x; i < room.x + room.width; i++)
    {
        to_fill[util_xy_to_i(i, room.y, map_width)] = 1;
        to_fill[util_xy_to_i(i, room.y + room.height - 1, map_width)] = 1;
    }

    // Draw vertical boundaries
    for (int i = room.y + 1; i < room.y + room.height - 1; i++)
    {
        to_fill[util_xy_to_i(room.x, i, map_width)] = 1;
        to_fill[util_xy_to_i(room.x + room.width - 1, i, map_width)] = 1;
    }
}

void _map_gen_line(
    char *to_fill, int map_width, int map_height,
    int start, int len, int pos, Direction dir)
{
    switch (dir)
    {
        case DIR_NORTH:
        case DIR_SOUTH:
        {
            if (start < 0 || start + len > map_height || pos >= map_width)
            {
                printf("_map_gen_line: WARNING: Line out of bounds. Skipping...\n");
                return;
            }

            for (int i = start; i < start + len; i++)
            {
                to_fill[util_xy_to_i(pos, i, map_width)] = 1;
            }
        } break;

        case DIR_EAST:
        case DIR_WEST:
        {
            if (start < 0 || start + len > map_width || pos >= map_height)
            {
                printf("_map_gen_line: WARNING: Line out of bounds. Skipping...\n");
                return;
            }

            for (int i = start; i < start + len; i++)
            {
                to_fill[util_xy_to_i(i, pos, map_width)] = 1;
            }
        } break;

        default:
        {

        } break;
    }
}

void _map_gen_from_prefab(
    char *to_fill, int map_width, int map_height,
    Rect rect, const char* prefab_path
)
{
    char* prefab = util_read_file(prefab_path);
    size_t prefab_i = 0;

    for (int y_i = rect.y; y_i < rect.y + rect.height; y_i++)
    {
        for (int x_i = rect.x; x_i < rect.x + rect.width; x_i++)
        {
            while (prefab[prefab_i] == '\n')
            {
                prefab_i++;
            }
            to_fill[util_xy_to_i(x_i, y_i, map_width)] = prefab[prefab_i] - '0'; // '0' -> 0
            prefab_i++;
        }
    }
}

// Rect -> 2 half rects. Order of rects_out depends on dir.
// If divided length is even, rects will be returned "wall-to-wall"
// if odd - overlapping.
void _map_rect_halves(Rect rect, Direction dir, Rect *rects_out)
{
    /*
        n = 10; n/2 = 5
        0123456789
        |...||...|
        aaaaabbbbb
        a width = 5 -> n/2   OR n-n/2
        b start = 5 -> n/2   OR n-n/2
        b width = 5 -> n-n/2 OR n/2

        n = 9; n/2 = 4:
        OPTION 1 (NO)------
        012345678
        |..||...|
        aaaabbbbb
        a width = 4 -> n/2
        b start = 4 -> n/2
        b width = 5 -> n-n/2
        ------
        OPTION 2 (YES)------
        012345678
        |...||..|
        aaaaabbbb
        a width = 5 -> n-n/2
        b start = 5 -> n-n/2
        b width = 4 -> n/2
    */

    Rect rect_a = { 0 };
    Rect rect_b = { 0 };

    switch (dir)
    {
        case DIR_NORTH:
        case DIR_SOUTH:
        {
            int h = rect.height;
            rect_a.x = rect.x;
            rect_a.y = rect.y;
            rect_a.width = rect.width;
            rect_a.height = h - h/2;
            rect_b.x = rect.x;
            rect_b.y = rect.y + (h - h/2);
            rect_b.width = rect.width;
            rect_b.height = h/2;
        } break;

        case DIR_EAST:
        case DIR_WEST:
        {
            int w = rect.width;
            rect_a.x = rect.x;
            rect_a.y = rect.y;
            rect_a.width = w - w/2;
            rect_a.height = rect.height;
            rect_b.x = rect.x + (w - w/2);
            rect_b.y = rect.y;
            rect_b.width = w/2;
            rect_b.height = rect.height;
        } break;

        default:
        {
        } break;
    }

    switch (dir)
    {
        case DIR_NORTH:
        case DIR_EAST:
        case DIR_SOUTH:
        case DIR_WEST:
        {
            //up-down and left-right, so no swap necessary
            rects_out[0] = rect_a;
            rects_out[1] = rect_b;
        } break;

        // case DIR_SOUTH:
        // case DIR_EAST:
        // {
        //     //down-up and right-left, swap
        //     rects_out[0] = rect_b;
        //     rects_out[1] = rect_a;
        // } break;

        default:
        {
            printf("_map_rect_halves: ERROR: Invalid direction.\n");
        } break;
    }
}

void map_gen_rect_room(Map *map, int width, int height)
{
    for (int i = 0; i <  width * height; i++)
    {
        map->glyphs[i] = 0xF9;
        glm_vec3_copy((vec3) { 0.05f, 0.05f, 0.05f }, map->fg_col[i]);
        glm_vec3_copy(GLM_VEC3_ZERO, map->bg_col[i]);
    }

    char *walls = calloc(1, width * height * sizeof(Glyph));
    Rect map_rect = { 0, 0, width, height };
    _map_gen_room(walls, width, height, map_rect);

    for (int i  = 0; i < width * height; i++)
    {
        if (walls[i] == 1)
        {
            map->glyphs[i] = '#';
            glm_vec3_copy((vec3) { 0.6f, 0.6f, 0.6f }, map->fg_col[i]);
            glm_vec3_copy((vec3) { 0.7f, 0.7f, 0.7f }, map->bg_col[i]);
            map->blocked[i] = true;
        }
    }

    free(walls);
}

void map_gen_test(Map *map, int width, int height)
{
    for (int i = 0; i <  width * height; i++)
    {
        map->glyphs[i] = 0xF9;
        glm_vec3_copy((vec3) { 0.25f, 0.25f, 0.25f }, map->fg_col[i]);
        glm_vec3_copy(GLM_VEC3_ZERO, map->bg_col[i]);
    }
    
    char *walls = calloc(1, width * height * sizeof(Glyph));
    Rect map_rect = { 0, 0, width, height };
    _map_gen_room(walls, width, height, map_rect);
    _map_gen_line(walls, width, height, 0, 10, 5, DIR_SOUTH);

    for (int i  = 0; i < width * height; i++)
    {
        if (walls[i] == 1)
        {
            map->glyphs[i] = '#';
            glm_vec3_copy((vec3) { 0.6f, 0.6f, 0.6f }, map->fg_col[i]);
            glm_vec3_copy((vec3) { 0.7f, 0.7f, 0.7f }, map->bg_col[i]);
            map->blocked[i] = true;
        }
    }

    free(walls);
}

void map_gen_level(Map *map, int width, int height)
{
    for (int i = 0; i <  width * height; i++)
    {
        map->glyphs[i] = 0xF9;
        glm_vec3_copy((vec3) { 0.05f, 0.05f, 0.05f }, map->fg_col[i]);
        glm_vec3_copy(GLM_VEC3_ZERO, map->bg_col[i]);
    }

    char *walls = calloc(1, width * height * sizeof(Glyph));

    Rect to_split = { 0, 0, width, height };

    int bsp_passes = 5;
    Direction dir = DIR_WEST;

    Rect *rooms_to_draw = calloc(1, (bsp_passes + 1) * sizeof(Rect));
    size_t r_iter = 0;

    for (int bsp_iter = 0; bsp_iter < bsp_passes; bsp_iter++)
    {
        Rect rooms[2] = {0};
        _map_rect_halves(to_split, dir, rooms);
        rooms_to_draw[r_iter] = rooms[0];
        r_iter++;
        if (bsp_iter == bsp_passes - 1)
        {
            rooms_to_draw[r_iter] = rooms[1];
            r_iter++;
        }
        to_split = rooms[1];
        dir = (dir + 1) % DIR_NONE;
    }
    
    _map_gen_from_prefab(walls, width, height, rooms_to_draw[0], "res/prefabs/lobby.txt");
    _map_gen_from_prefab(walls, width, height, rooms_to_draw[1], "res/prefabs/office.txt");
    _map_gen_from_prefab(walls, width, height, rooms_to_draw[2], "res/prefabs/cafe.txt");
    _map_gen_from_prefab(walls, width, height, rooms_to_draw[3], "res/prefabs/lounge.txt");
    _map_gen_from_prefab(walls, width, height, rooms_to_draw[4], "res/prefabs/meeting.txt");
    _map_gen_from_prefab(walls, width, height, rooms_to_draw[5], "res/prefabs/executive.txt");

    // for (int i = 5; i < bsp_passes + 1; i++)
    // {
    //     _map_gen_room(walls, width, height, rooms_to_draw[i]);
    // }

    free (rooms_to_draw);

    vec3 lobby_bg = { 0.05f, 0.05f, 0.05f };
    for (int i  = 0; i < width * height; i++)
    {
        if (walls[i] == 74) // lobby floor
        {
            map->glyphs[i] = 0xFE;
            glm_vec3_copy((vec3) { 0.1f, 0.1f, 0.1f }, map->fg_col[i]);
            glm_vec3_copy(lobby_bg, map->bg_col[i]);
            map->blocked[i] = false;
        }
        else if (walls[i] == 1) // wall
        {
            map->glyphs[i] = '#';
            glm_vec3_copy((vec3) { 0.6f, 0.6f, 0.6f }, map->fg_col[i]);
            glm_vec3_copy((vec3) { 0.7f, 0.7f, 0.7f }, map->bg_col[i]);
            map->blocked[i] = true;
        }
        else if (walls[i] == 2) // table
        {
            map->glyphs[i] = 0xD2;
            glm_vec3_copy((vec3) { 0.5f, 0.1f, 0.1f }, map->fg_col[i]);
            glm_vec3_copy(lobby_bg, map->bg_col[i]);
            map->blocked[i] = true;
        }
        else if (walls[i] == 3) // chair
        {
            map->glyphs[i] = 0x7F;
            glm_vec3_copy((vec3) { 0.3f, 0.3f, 0.1f }, map->fg_col[i]);
            glm_vec3_copy(lobby_bg, map->bg_col[i]);
            map->blocked[i] = false;
        }
        else if (walls[i] == 4) // bush
        {
            map->glyphs[i] = 0xB0;
            glm_vec3_copy((vec3) { 0.1f, 0.6f, 0.1f }, map->fg_col[i]);
            glm_vec3_copy(lobby_bg, map->bg_col[i]);
            map->blocked[i] = false;
        }
        else if (walls[i] == 5) // Blue flower
        {
            map->glyphs[i] = 0x0F;
            glm_vec3_copy((vec3) { 0.1f, 0.1f, 0.6f }, map->fg_col[i]);
            glm_vec3_copy(lobby_bg, map->bg_col[i]);
            map->blocked[i] = true;
        }
        else if (walls[i] == 6) // couch
        {
            map->glyphs[i] = 0xB1;
            glm_vec3_copy((vec3) { 1.0f, 0.1f, 0.1f }, map->fg_col[i]);
            glm_vec3_copy(lobby_bg, map->bg_col[i]);
            map->blocked[i] = false;
        }
        else if (walls[i] == 7) // door (blocked)
        {
            map->glyphs[i] = 0xD7;
            glm_vec3_copy((vec3) { 0.1f, 0.1f, 0.1f }, map->fg_col[i]);
            glm_vec3_copy((vec3) { 0.5f, 0.5f, 0.5f }, map->bg_col[i]);
            map->blocked[i] = true;
        }
        else if (walls[i] == 8) // window outside
        {
            map->glyphs[i] = 0xCE;
            glm_vec3_copy((vec3) { 0.5f, 0.6f, 0.8f }, map->fg_col[i]);
            glm_vec3_copy((vec3) { 0.7f, 0.7f, 0.7f }, map->bg_col[i]);
            map->blocked[i] = true;
        }
        else if (walls[i] == 9) // Screen
        {
            map->glyphs[i] = 0x08;
            glm_vec3_copy((vec3) { 0.7f, 0.7f, 0.7f }, map->fg_col[i]);
            glm_vec3_copy(lobby_bg, map->bg_col[i]);
            map->blocked[i] = true;
        }
        else if (walls[i] == 10) // Cubicle walls
        {
            map->glyphs[i] = 0xB2;
            glm_vec3_copy((vec3) { 0.5f, 0.5f, 0.5f }, map->fg_col[i]);
            glm_vec3_copy(lobby_bg, map->bg_col[i]);
            map->blocked[i] = true;
        }
        else if (walls[i] == 11) // Music box
        {
            map->glyphs[i] = 0x0E;
            glm_vec3_copy((vec3) { 0.5f, 0.0f, 0.5f }, map->fg_col[i]);
            glm_vec3_copy(lobby_bg, map->bg_col[i]);
            map->blocked[i] = true;
        }
        else if (walls[i] == 12) // Red flower
        {
            map->glyphs[i] = 0x0F;
            glm_vec3_copy((vec3) { 1.0f, 0.0f, 0.1f }, map->fg_col[i]);
            glm_vec3_copy(lobby_bg, map->bg_col[i]);
            map->blocked[i] = true;
        }
    }

    free(walls);
}
