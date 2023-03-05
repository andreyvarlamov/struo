#include "main.h"

typedef struct Entity
{
    int x;
    int y;
    vec3 bg;
    vec3 fg; 
    Glyph glyph;
    bool alive;
} Entity;