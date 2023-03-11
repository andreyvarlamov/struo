#include "main.h"

typedef struct Point
{
    int x;
    int y;
} Point;

typedef struct Rect
{
    int x;
    int y;
    int width;
    int height;
} Rect;

typedef enum Direction
{
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST,
    DIR_NONE
} Direction;

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

size_t util_p_to_i(Point p, int width)
{
    return (size_t) (p.y * width + p.x);
}

Point util_i_to_p(size_t i, int width)
{
    Point p;

    p.x = (int) i % width;
    p.y = (int) i / width;

    return p;
}

bool util_p_cmp(Point p1, Point p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

Point util_xy_to_p(int x, int y)
{
    Point p = { .x = x, .y = y };
    return p;
}

int util_calc_sqr_distance(int x1, int y1, int x2, int y2)
{
    int d_x = x1 - x2;
    int d_y = y1 - y2;

    return d_x * d_x + d_y * d_y;
}

bool util_check_p_within_rad(Point pos, int radius, Point goal)
{
    Point d = { abs(pos.x - goal.x), abs(pos.y - goal.y) };

    return (d.x * d.x + d.y * d.y) < (radius * radius);
}

bool util_check_p_in_bounds(Point p, int map_width, int map_height)
{
    return p.x >= 0 && p.y >= 0 && p.x < map_width && p.y < map_height;
}

bool util_check_rect_in_bounds(Rect rect, int map_width, int map_height)
{
    return rect.x >= 0 && rect.y >= 0
        && rect.x + rect.width-1 < map_width
        && rect.y + rect.height-1 < map_height;
}

typedef struct AString
{
    char str[32];
} AString;
