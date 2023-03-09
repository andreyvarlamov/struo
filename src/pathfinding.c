#include "main.h"

// blocked, map_width, map_height - from map data
// path from s to g
Point pathfinding_bfs(
    bool *blocked, int map_width, int map_height,
    Point s, Point g
)
{
    // HACKY: Don't count goal as blocked while calculating. Set back in the end of func
    bool old_blocked = blocked[util_xy_to_i(g.x, g.y, map_width)];
    blocked[util_xy_to_i(g.x, g.y, map_width)] = false;

    bool *visited = calloc(1, map_width * map_height * sizeof(bool));

    Point *queue = calloc(1, map_width * map_height * sizeof(Point));
    int q_s = 0;
    int q_e = 0;

    visited[util_xy_to_i(s.x, s.y, map_width)] = true;
    queue[q_e].x = s.x;
    queue[q_e].y = s.y;
    q_e++;

    Point *path = calloc(1, map_width * map_height * sizeof(Point));

    bool found = false;
    Point found_p = { -1, -1 };

    while (q_s < q_e)
    {

        Point curr = queue[q_s];
        q_s++;
        Point neighbors[4] = {0};

        neighbors[0].x = curr.x - 1;
        neighbors[0].y = curr.y;
        neighbors[1].x = curr.x + 1;
        neighbors[1].y = curr.y;
        neighbors[2].x = curr.x;
        neighbors[2].y = curr.y - 1;
        neighbors[3].x = curr.x;
        neighbors[3].y = curr.y + 1;

        for (int n_i = 0; n_i < 4; n_i++)
        {
            Point nbr = neighbors[n_i];

            // Check it's not out of bounds and not blocked
            if (nbr.x >= 0 && nbr.y >= 0 && nbr.x < map_width && nbr.y < map_height
                && !blocked[util_xy_to_i(nbr.x, nbr.y, map_width)])
            {
                // Check it has not been visited
                if (!visited[util_xy_to_i(nbr.x, nbr.y, map_width)])
                {
                    // Remember parent node
                    path[util_xy_to_i(nbr.x, nbr.y, map_width)].x = curr.x;
                    path[util_xy_to_i(nbr.x, nbr.y, map_width)].y = curr.y;

                    // If this node is the goal, we're done
                    if (nbr.x == g.x && nbr.x && nbr.y == g.y)
                    {
                        found = true;
                        found_p.x = nbr.x;
                        found_p.y = nbr.y;
                        break;
                    }

                    // Add to the end of the queue
                    queue[q_e].x = nbr.x;
                    queue[q_e].y = nbr.y;
                    q_e++;

                    // Add to visited
                    visited[util_xy_to_i(nbr.x, nbr.y, map_width)] = true;
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
            prev.x = cursor.x;
            prev.y = cursor.y;
            cursor.x = path[util_xy_to_i(prev.x, prev.y, map_width)].x;
            cursor.y = path[util_xy_to_i(prev.x, prev.y, map_width)].y;
        }
        // printf("Next step: %d, %d", prev.x, prev.y);
    }

    blocked[util_xy_to_i(g.x, g.y, map_width)] = old_blocked;

    free(visited);
    free(queue);
    free(path);

    return prev;
}
