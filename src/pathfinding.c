#include "main.h"

// blocked, map_width, map_height - from map data
// path from s to g
void pathfinding_bfs(
    bool *blocked, int map_width, int map_height,
    Point s, Point g
)
{
    // HACKY: Don't count goal as blocked while calculating. Set back in the end of func
    bool old_blocked = blocked[util_xy_to_i(g.x, g.y, map_width)];
    blocked[util_xy_to_i(g.x, g.y, map_width)] = false;

    Point *visited = calloc(1, map_width * map_height * sizeof(Point));
    Point *queue = calloc(1, map_width * map_height * sizeof(Point));
    int v_i = 0;
    int q_s = 0;
    int q_e = 0;

    visited[v_i].x = s.x;
    visited[v_i].y = s.y;
    v_i++;
    queue[q_e].x = s.x;
    queue[q_e].y = s.y;
    q_e++;

    bool found = false;

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
                bool vis = false;
                for (int i = 0; i < v_i; i++)
                {
                    if(visited[i].x == nbr.x && visited[i].y == nbr.y)
                    {
                        vis = true;
                        break;
                    }
                }

                if (!vis)
                {
                    if (nbr.x == g.x && nbr.x && nbr.y == g.y)
                    {
                        found = true;
                        break;
                    }

                    // Add to visited
                    visited[v_i].x = nbr.x;
                    visited[v_i].y = nbr.y;
                    v_i++;

                    queue[q_e].x = nbr.x;
                    queue[q_e].y = nbr.y;
                    q_e++;
                }
            }
        }

        if (found)
        {
            break;
        }
    }

    blocked[util_xy_to_i(g.x, g.y, map_width)] = old_blocked;
}
