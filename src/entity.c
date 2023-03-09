#include "main.h"

typedef struct Entity
{
    Point pos;
    vec3 bg;
    vec3 fg; 
    Glyph glyph;
    bool alive;
} Entity;

// bool entity_check_within_fov(Map *map, Point )
