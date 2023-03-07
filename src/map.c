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
    Glyph *to_fill, int map_width, int map_height, Glyph glyph,
    int x, int y, int width, int height)
{
    if (x < 0 || y < 0 || x >= width || y >= height
        || x + width > map_width || y + height > map_height
    )
    {
        printf("_map_gen_rooms: WARNING: Room out of bounds. Skipping...\n");
        return;
    }

    // Draw horizontal boundaries
    for (int i = x; i < x + width; i++)
    {
        to_fill[util_xy_to_i(i, y, map_width)] = glyph;
        to_fill[util_xy_to_i(i, y + height - 1, map_width)] = glyph;
    }

    // Draw vertical boundaries
    for (int i = y + 1; i < y + height - 1; i++)
    {
        to_fill[util_xy_to_i(x, i, map_width)] = glyph;
        to_fill[util_xy_to_i(x + width - 1, i, map_width)] = glyph;
    }
}

void _map_gen_line(
    Glyph *to_fill, int map_width, int map_height, Glyph glyph,
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
                to_fill[util_xy_to_i(pos, i, map_width)] = glyph;
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
                to_fill[util_xy_to_i(i, pos, map_width)] = glyph;
            }
        } break;

        default:
        {

        } break;
    }
}

void _map_gen_cubicle_section(
    Glyph *to_fill, int map_width, int map_height, Glyph glyph,
    int x, int y, Direction entry_dir, int width, int length, int cubicle_num
)
{
    switch (entry_dir)
    {
        case DIR_NORTH:
        {
            _map_gen_line(to_fill, map_width, map_height, glyph, y, length, x, DIR_NORTH);
            _map_gen_line(to_fill, map_width, map_height, glyph, y, length, x + width - 1, DIR_NORTH);
            _map_gen_line(to_fill, map_width, map_height, glyph, x, width, y + length, DIR_EAST);

            int cubicle_offset = width / cubicle_num;
            int start = x + cubicle_offset;
            for (int i = 0; i < cubicle_num - 1; i++)
            {
                _map_gen_line(to_fill, map_width, map_height, glyph, y, length, start, DIR_NORTH);
                start += cubicle_offset;
            }
        } break;

        case DIR_EAST:
        {
            _map_gen_line(to_fill, map_width, map_height, glyph, x, length, y, DIR_EAST);
            _map_gen_line(to_fill, map_width, map_height, glyph, x, length, y + width - 1, DIR_EAST);
            _map_gen_line(to_fill, map_width, map_height, glyph, y, width, x, DIR_NORTH);

            int cubicle_offset = width / cubicle_num;
            int start = y + cubicle_offset;
            for (int i = 0; i < cubicle_num - 1; i++)
            {
                _map_gen_line(to_fill, map_width, map_height, glyph, x, length, start, DIR_EAST);
                start += cubicle_offset;
            }
        } break;

        default:
        {

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

    Glyph *walls = calloc(1, width * height * sizeof(Glyph));
    _map_gen_room(walls, width, height, '#', 0, 0, width, height);

    for (int i  = 0; i < width * height; i++)
    {
        if (walls[i] == '#')
        {
            map->glyphs[i] = walls[i];
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

    Glyph *walls = calloc(1, width * height * sizeof(Glyph));

    int bsp_passes = 7;

    int room_x = 0;
    int room_y = 0;
    int room_width = width;
    int room_height = height;

    int type = rand() % 2;

    for (int i = 0; i < bsp_passes; i++)
    {
        if (type)
        {
            int new_width = room_width % 2 == 0 ? room_width / 2 : room_width / 2 + 0;
            _map_gen_line(walls, width, height, '#', room_y, room_height, room_x + new_width, DIR_NORTH);
            room_x = room_x + new_width;
            room_width = new_width - (room_width % 2 == 0 ? 0 : 1);
        }
        else
        {
            int new_height = room_height % 2 == 0 ? room_height / 2 : room_height / 2 + 0;
            _map_gen_line(walls, width, height, '#', room_x, room_width, room_y + new_height, DIR_EAST);
            room_y = room_y + new_height;
            room_height = new_height - (room_height % 2 == 0 ? 0 : 1);
        }
        type = (type + 1) % 2;
    }

    for (int i  = 0; i < width * height; i++)
    {
        if (walls[i] == '#')
        {
            map->glyphs[i] = '#';
            glm_vec3_copy((vec3) { 0.6f, 0.6f, 0.6f }, map->fg_col[i]);
            glm_vec3_copy((vec3) { 0.7f, 0.7f, 0.7f }, map->bg_col[i]);
            map->blocked[i] = true;
        }
    }

    free(walls);
}

void map_gen_draft(Map *map, int width, int height)
{
    for (int i = 0; i <  width * height; i++)
    {
        map->glyphs[i] = 0xF9;
        glm_vec3_copy((vec3) { 0.05f, 0.05f, 0.05f }, map->fg_col[i]);
        glm_vec3_copy(GLM_VEC3_ZERO, map->bg_col[i]);
    }

    Glyph *walls = calloc(1, width * height * sizeof(Glyph));

    _map_gen_room(walls, width, height, '#', 0, 0, width, height);
    
    _map_gen_line(walls, width, height, '#', 3, 10, 5, DIR_NORTH);
    _map_gen_line(walls, width, height, '#', 5, 10, 13, DIR_EAST);

    _map_gen_cubicle_section(walls, width, height, '#', 5, 32, DIR_NORTH, 40, 10, 5);
    _map_gen_cubicle_section(walls, width, height, '#', 50, 5, DIR_EAST, 40, 10, 5);

    for (int i  = 0; i < width * height; i++)
    {
        if (walls[i] == '#')
        {
            map->glyphs[i] = '#';
            glm_vec3_copy((vec3) { 0.6f, 0.6f, 0.6f }, map->fg_col[i]);
            glm_vec3_copy((vec3) { 0.7f, 0.7f, 0.7f }, map->bg_col[i]);
            map->blocked[i] = true;
        }
    }

    free(walls);
}
