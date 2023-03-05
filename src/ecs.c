#include "main.h"

typedef struct Position
{
    int x;
    int y;
} Position;

typedef struct Renderable
{
    vec3 fg;
    vec3 bg;
    Glyph glyph;
} Renderable;

typedef enum ComponentType
{
    POSITION,
    RENDERABLE
} ComponentType;

typedef union ComponentUnion
{
    Position position;
    Renderable renderable;
} ComponentUnion;

typedef struct Component
{
    ComponentUnion d;
    ComponentType type;
    bool active;
} Component;

typedef struct ECS
{
    size_t tot;
    size_t cap;
    bool *active;
    Component *position;
    Component *renderable;
} ECS;

void ecs_init(ECS *ecs)
{
    ecs->tot = 0;
    ecs->cap = ENTITY_INIT_COUNT;

    ecs->active     = (bool *)      calloc(1, ENTITY_INIT_COUNT * sizeof(bool));

    ecs->position   = (Component *) calloc(1, ENTITY_INIT_COUNT * sizeof(Component));
    ecs->renderable = (Component *) calloc(1, ENTITY_INIT_COUNT * sizeof(Component));
}

size_t ecs_add_entity(ECS *ecs)
{
    size_t i = ecs->tot;
    ecs->active[i] = true;
    ecs->tot++;

    // TODO: realloc logic

    return i;
}

void ecs_add_component(ECS *ecs, size_t id, Component *component)
{
    if(id >= ecs->tot)
    {
        printf("ERROR: ECS Index out of range");
    }

    if(!ecs->active[id])
    {
        printf("ERROR: Addding component to an inactive entity.");
    }

    switch (component->type)
    {
        case POSITION:
        {
            memcpy(&ecs->position[id], component, sizeof(Component));
        } break;

        case RENDERABLE:
        {
            memcpy(&ecs->renderable[id], component, sizeof(Component));
        } break;

        default:
        {
            printf("ERROR: Tried to add unknown component type");
        } break;
    }
}

void ecs_get_component(ECS *ecs, size_t id, ComponentType type, Component *component_out)
{
    if(id >= ecs->tot)
    {
        printf("ERROR: ECS Index out of range");
    }

    if(!ecs->active[id])
    {
        printf("ERROR: Getting component for an inactive entity.");
    }

    switch (type)
    {
        case POSITION:
        {
            memcpy(component_out, &ecs->position[id], sizeof(Component));
        } break;

        case RENDERABLE:
        {
            memcpy(component_out, &ecs->renderable[id], sizeof(Component));
        } break;

        default:
        {
            printf("ERROR: Tried to get unknown component type");
        } break;
    }
}

void ecs_query_entities(
    ECS *ecs, size_t type_count, ComponentType types[],
    Component *components_out[], size_t ids[], int *entity_count_out)
{
    Component **iters = (Component **) calloc(1, type_count * sizeof(Component *));

    // size_t *ids = (size_t *) calloc(1, )

    for (size_t i = 0; i < ecs->tot; i++)
    {
        bool match = true;
        for (size_t j = 0; j < type_count; j++)
        {
            match &= iters[j][i].active;
        }

        if (match)
        {
            ;
        }
    }
}

void ecs_clean(ECS *ecs)
{
    ecs->tot = 0;
    ecs->cap = 0;

    free(ecs->active);
    free(ecs->position);
    free(ecs->renderable);
}