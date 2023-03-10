#include "main.h"

typedef struct Stats
{
    char name[24];

    int health; // 0-max_health
    int max_health; // 100 by default

    int accuracy; // 1-10
    int evasion; // 1-10

    int damage;
    int defense;

    int speed;
} Stats;

Stats combat_stats_ctor(const char name[24],
                        int health, int max_health,
                        int accuracy, int evasion,
                        int damage, int defense,
                        int speed)
{
    Stats stats;

    strcpy(stats.name, name);
    stats.health = health;
    stats.max_health = max_health;
    stats.accuracy = accuracy;
    stats.evasion = evasion;
    stats.damage = damage;
    stats.defense = defense;
    stats.speed = speed;

    return stats;
}

void combat_attack(Stats *att, Stats *def)
{
    int hit = rand() % 100;

    int chance_d = att->accuracy - def->evasion;

    float dam_coef;

    char *hit_type = malloc(16 * sizeof(char));

    if (hit > (98 - chance_d))
    {
        // critical
        dam_coef = 1.5f;
        strcpy(hit_type, "(!) crit");
    }
    else if (hit > (40 - (chance_d * 4)))
    {
        // hit
        dam_coef = 1.0f;
        strcpy(hit_type, "hit");
    }
    else
    {
        // miss
        dam_coef = 0.0f;
        strcpy(hit_type, "miss");
    }

    int end_dmg = (int) (dam_coef * (att->damage - def->defense));
    def->health -= end_dmg;

    printf("%s attacks %s. It's a %s for %d.\n", att->name, def->name, hit_type, end_dmg);

    free(hit_type);

    // TODO: handle death
}

/*
    damage - what kind of weapon
    accuracy - how skilled are you; buff items
    defense, speed and evasion - depend on armor item
    evasion - buff items
*/
