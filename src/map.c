#include "main.h"

typedef struct Map
{
    vec3 fg_col[SCREEN_COLS * SCREEN_ROWS];
    vec3 bg_col[SCREEN_COLS * SCREEN_ROWS];
    Glyph glyphs[SCREEN_COLS * SCREEN_ROWS];
    bool blocked[SCREEN_COLS * SCREEN_ROWS];
} Map;

// Get indices to draw in for a rectangle. Pass array on heap with existing
// indices. It will add more to add another room, and update count.
void _map_get_room_indices(
    int x, int y, int width, int height, int map_width, int map_height,
    int **indices_in_out, size_t *count_in_out
)
{
    if (!util_check_xy_within_bounds(x, y, map_width, map_height)
        || !util_check_xy_within_bounds(x + width - 1, y + height - 1, map_width, map_height)
        || width < 3 || height < 3
    )
    {
        printf("_map_get_room_indices: Out of bounds.\n");
        return;
    }

    int count = width * 2 + height * 2 - 4;
    // int count = width * 2;
    int old_count = *count_in_out;

    int new_count = *count_in_out + count;

    int *temp_indices = realloc(*indices_in_out, new_count * sizeof(int));
    
    if (!temp_indices)
    {
        printf("_map_get_room_indices: Could not realloc.\n");
        return;
    }

    *indices_in_out = temp_indices;

    int indices_iter = old_count;

    // Draw horizontal boundaries
    for (int i = x; i < x + width; i++)
    {
        (*indices_in_out)[indices_iter] = util_xy_to_i(i, y, map_width);
        indices_iter++;
        (*indices_in_out)[indices_iter] = util_xy_to_i(i, y + height - 1, map_width);
        indices_iter++;
    }

    // Draw vertical boundaries
    for (int i = y + 1; i < y + height - 1; i++)
    {
        (*indices_in_out)[indices_iter] = util_xy_to_i(x, i, map_width);
        indices_iter++;
        (*indices_in_out)[indices_iter] = util_xy_to_i(x + width - 1, i, map_width);
        indices_iter++;
    }

    *count_in_out = new_count;
}

void map_gen_rect(Map *map, int width, int height)
{
    for (int i = 0; i <  width * height; i++)
    {
        map->glyphs[i] = 0xF9;
        glm_vec3_copy((vec3) { 0.05f, 0.05f, 0.05f }, map->fg_col[i]);
        glm_vec3_copy(GLM_VEC3_ZERO, map->bg_col[i]);
    }

    int *indices_to_draw = NULL;
    size_t index_count = 0;
    _map_get_room_indices(
        0, 0, width, height, width, height,
        &indices_to_draw, &index_count
    );
    
    for (size_t i  = 0; i < index_count; i++)
    {
        map->glyphs[indices_to_draw[i]] = '#';
        glm_vec3_copy((vec3) { 0.6f, 0.6f, 0.6f }, map->fg_col[indices_to_draw[i]]);
        glm_vec3_copy((vec3) { 0.7f, 0.7f, 0.7f }, map->bg_col[indices_to_draw[i]]);
        map->blocked[indices_to_draw[i]] = true;
    }

    free(indices_to_draw);
}

void map_gen_level(Map *map, int width, int height)
{
    // Fill room with floor
    for (int i = 0; i <  width * height; i++)
    {
        map->glyphs[i] = 0xF9;
        glm_vec3_copy((vec3) { 0.05f, 0.05f, 0.05f }, map->fg_col[i]);
        glm_vec3_copy(GLM_VEC3_ZERO, map->bg_col[i]);
    }

    // Create outer wall
    int *indices_to_draw = NULL;
    size_t index_count = 0;
    _map_get_room_indices(
        0, 0, width, height, width, height,
        &indices_to_draw, &index_count
    );

    // Add room walls 
    int room_num = 5 + (rand() % 10);

    for (int i = 0; i < room_num; i++)
    {
        int room_width = 3 + (rand() % 8);
        int room_height = 3 + (rand() % 8);

        int x = rand() % (width - room_width + 1);
        int y = rand() % (height - room_height + 1);

        _map_get_room_indices(
            x, y, room_width, room_height, width, height,
            &indices_to_draw, &index_count
        );
    }

    for (size_t i  = 0; i < index_count; i++)
    {
        map->glyphs[indices_to_draw[i]] = '#';
        glm_vec3_copy((vec3) { 0.6f, 0.6f, 0.6f }, map->fg_col[indices_to_draw[i]]);
        glm_vec3_copy((vec3) { 0.7f, 0.7f, 0.7f }, map->bg_col[indices_to_draw[i]]);
        map->blocked[indices_to_draw[i]] = true;
    }

    // Add doors
    // Add windows
}
