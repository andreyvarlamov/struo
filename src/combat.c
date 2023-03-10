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

typedef struct AttackResult
{
    char hit_type[16];
    int end_dmg;
} AttackResult;

Stats combat_stats_ctor(const char name[24],
                        int max_health,
                        int accuracy, int evasion,
                        int damage, int defense,
                        int speed)
{
    Stats stats;

    strcpy(stats.name, name);
    stats.health = max_health;
    stats.max_health = max_health;
    stats.accuracy = accuracy;
    stats.evasion = evasion;
    stats.damage = damage;
    stats.defense = defense;
    stats.speed = speed;

    return stats;
}

AttackResult combat_attack(Stats *att, Stats *def)
{
    int hit = rand() % 100;

    int chance_d = att->accuracy - def->evasion;

    float dam_coef;

    char hit_type[16] = {0};

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

    int def_modifier = 100 - def->defense;
    if (def_modifier < 5)
    {
        def_modifier = 5;
    }

    int randomized_dmg = rand() % (att->damage / 3) - att->damage / 6 + att->damage;

    int end_dmg = (int) (dam_coef * (randomized_dmg * def_modifier * 0.01f));
    def->health -= end_dmg;

    AttackResult ar;
    strcpy(ar.hit_type, hit_type);
    ar.end_dmg = end_dmg;

    return ar;
}

/*
    damage - what kind of weapon
    accuracy - how skilled are you; buff items
    defense, speed and evasion - depend on armor item
    evasion - buff items
*/
