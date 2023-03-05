#include "main.h"

typedef struct Map
{
    vec3 fg_col[SCREEN_COLS * SCREEN_ROWS];
    vec3 bg_col[SCREEN_COLS * SCREEN_ROWS];
    Glyph glyphs[SCREEN_COLS * SCREEN_ROWS];
    bool blocked[SCREEN_COLS * SCREEN_ROWS];
} Map;

void map_init(Map *map, int width, int height)
{
    for (int i = 0; i <  width * height; i++)
    {
        map->glyphs[i] = 0xF9;
        glm_vec3_copy((vec3) { 0.05f, 0.05f, 0.05f }, map->fg_col[i]);
        glm_vec3_copy(GLM_VEC3_ZERO, map->bg_col[i]);
    }

    int wall_tiles_count = width * 2 + height * 2 - 4;
    int *wall_indeces = (int *) calloc(1, wall_tiles_count * sizeof(int));

    int wall_indeces_iter = 0;

    // Draw horizontal boundaries
    for (int i = 0; i < width; i++)
    {
        wall_indeces[wall_indeces_iter] = i;
        wall_indeces_iter++;
        wall_indeces[wall_indeces_iter] = util_xy_to_i(i, height - 1, width);
        wall_indeces_iter++;
    }

    // Draw vertical boundaries
    for (int i = 1; i < height - 1; i++)
    {
        wall_indeces[wall_indeces_iter] = util_xy_to_i(0, i, width);
        wall_indeces_iter++;
        wall_indeces[wall_indeces_iter] = util_xy_to_i(width - 1, i, width);
        wall_indeces_iter++;
    }
    
    for (int i  = 0; i < wall_tiles_count; i++)
    {
        map->glyphs[wall_indeces[i]] = '#';
        glm_vec3_copy((vec3) { 0.6f, 0.6f, 0.6f }, map->fg_col[wall_indeces[i]]);
        glm_vec3_copy((vec3) { 0.7f, 0.7f, 0.7f }, map->bg_col[wall_indeces[i]]);
        map->blocked[wall_indeces[i]] = true;
    }

    free(wall_indeces);
}