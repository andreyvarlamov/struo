#include "main.h"

typedef struct Map
{
    vec3 fg_col[SCREEN_COLS * SCREEN_ROWS];
    vec3 bg_col[SCREEN_COLS * SCREEN_ROWS];
    Glyph glyphs[SCREEN_COLS * SCREEN_ROWS];
    bool blocked[SCREEN_COLS * SCREEN_ROWS];
} Map;

// to_fill should come preallocated and initialized
void _map_gen_rooms(
    unsigned char *to_fill, int map_width, int map_height,
    int x, int y, int width, int height)
{
    if (x < 0 || y < 0 || x >= width || y >= height
        || x + width > map_width || y + height > map_height
    )
    {
        printf("_map_gen_rooms: WARNING: Room out of bounds. Skipping...");
        return;
    }

    // Draw horizontal boundaries
    for (int i = x; i < x + width; i++)
    {
        to_fill[util_xy_to_i(i, y, map_width)] = 1;
        to_fill[util_xy_to_i(i, y + height - 1, map_width)] = 1;
    }

    // Draw vertical boundaries
    for (int i = y + 1; i < y + height - 1; i++)
    {
        to_fill[util_xy_to_i(x, i, map_width)] = 1;
        to_fill[util_xy_to_i(x + width - 1, i, map_width)] = 1;
    }
}

void map_init(Map *map, int width, int height)
{
    for (int i = 0; i <  width * height; i++)
    {
        map->glyphs[i] = 0xF9;
        glm_vec3_copy((vec3) { 0.05f, 0.05f, 0.05f }, map->fg_col[i]);
        glm_vec3_copy(GLM_VEC3_ZERO, map->bg_col[i]);
    }

    unsigned char *walls = calloc(1, width * height * sizeof(unsigned char));
    _map_gen_rooms(walls, width, height, 0, 0, width, height);

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
