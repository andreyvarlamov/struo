#include "main.h"

// blocked, map_width, map_height - from map data
// path from s to g
Point pathfinding_bfs(
    const bool *collisions, int map_width, int map_height,
    Point s, Point g
)
{
    bool *blocked = malloc(map_width * map_height * sizeof(bool));
    memcpy(blocked, collisions, map_width * map_height * sizeof(bool));
    blocked[util_p_to_i(g, map_width)] = false; // HACKY

    bool *visited = calloc(1, map_width * map_height * sizeof(bool));

    Point *queue = calloc(1, map_width * map_height * sizeof(Point));
    int q_s = 0;
    int q_e = 0;

    visited[util_p_to_i(s, map_width)] = true;
    queue[q_e] = util_xy_to_p(s.x, s.y);
    q_e++;

    Point *path = calloc(1, map_width * map_height * sizeof(Point));

    bool found = false;
    Point found_p;

    while (q_s < q_e)
    {
        Point curr = queue[q_s];
        q_s++;

        Point neighbors[4];
        neighbors[0] = util_xy_to_p(curr.x - 1, curr.y);
        neighbors[1] = util_xy_to_p(curr.x + 1, curr.y);
        neighbors[2] = util_xy_to_p(curr.x, curr.y - 1);
        neighbors[3] = util_xy_to_p(curr.x, curr.y + 1);

        for (int n_i = 0; n_i < 4; n_i++)
        {
            Point n = neighbors[n_i];

            // Check it's not out of bounds and not blocked
            if (util_check_p_in_bounds(n, map_width, map_height) && !blocked[util_p_to_i(n, map_width)])
            {
                // Check it has not been visited
                if (!visited[util_p_to_i(n, map_width)])
                {
                    // Remember parent node
                    path[util_p_to_i(n, map_width)].x = curr.x;
                    path[util_p_to_i(n, map_width)].y = curr.y;

                    // If this node is the goal, we're done
                    if (util_p_cmp(n, g))
                    {
                        found = true;
                        found_p = util_xy_to_p(n.x, n.y);
                        break;
                    }

                    // Add to the end of the queue
                    queue[q_e] = util_xy_to_p(n.x, n.y);
                    q_e++;

                    // Add to visited
                    visited[util_p_to_i(n, map_width)] = true;
                }
            }
        }

        if (found)
        {
            break;
        }
    }

    Point prev = { -1, -1 };
    if (found)
    {
        Point cursor = { found_p.x, found_p.y };

        while (cursor.x != s.x || cursor.y != s.y)
        {
            prev = util_xy_to_p(cursor.x, cursor.y);
            Point parent = path[util_p_to_i(prev, map_width)];
            cursor = util_xy_to_p(parent.x, parent.y);
        }
    }

    free(visited);
    free(queue);
    free(path);
    free(blocked);

    return prev;
}
