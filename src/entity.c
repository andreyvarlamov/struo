#include "main.h"

typedef struct Entity
{
    Point pos;
    vec3 bg;
    vec3 fg; 
    Glyph glyph;
    bool alive;
} Entity;

bool entity_check_pos_within_fov(bool *coll, int map_width, int map_height, 
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

    // HACKY: Don't count goal as blocked while calculating. Set back in the end of func
    bool old_blocked = coll[util_p_to_i(goal, map_width)];
    coll[util_p_to_i(goal, map_width)] = false;

    for(Point i = { 0, 0 }; i.x < n.x || i.y < n.y;)
    {
        if ((1 + 2 * i.x) * n.y < (1 + 2 * i.y) * n.x)
        {
            pos.x += sign.x;
            i.x++;
        }
        else
        {
            pos.y += sign.y;
            i.y++;
        }

        if (coll[util_p_to_i(pos, map_width)])
        {
            return false;
        }
    }

    coll[util_p_to_i(goal, map_width)] = old_blocked;

    return true;
}
