#include "main.h"

char *util_read_file(const char *path)
{
    FILE *f;
    long len;
    char* content;

    f = fopen(path, "rb");
    if (!f)
    {
        printf("Could not open file at %s\n", path);
        exit(-1);
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    content = (char*) calloc(1, len+1);
    fread(content, 1, len, f);
    content[len] = 0; // Null terminate. TODO: Make sure this works properly
    fclose(f);

    return content;
}

size_t util_xy_to_i(int x, int y, int width)
{
    return (size_t) (y * width + x);
}

void util_i_to_xy(size_t i, int width, int *x_out, int *y_out)
{
    *x_out = (int) i % width;
    *y_out = (int) i / width;
}

typedef enum Direction
{
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST,
    DIR_NONE
} Direction;

int util_calc_sqr_distance(int x1, int y1, int x2, int y2)
{
    int d_x = x1 - x2;
    int d_y = y1 - y2;

    return d_x * d_x + d_y * d_y;
}

typedef struct Rect
{
    int x;
    int y;
    int width;
    int height;
} Rect;

bool util_check_rect_in_bounds(Rect rect, int map_width, int map_height)
{
    return rect.x >= 0 && rect.y >= 0
        && rect.x + rect.width-1 < map_width
        && rect.y + rect.height-1 < map_height;
}
