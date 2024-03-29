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

    ArmorType armor;
    GunType gun;
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
                        int speed,
                        ArmorType armor,
                        GunType gun)
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

    stats.armor = armor;
    stats.gun = gun;

    return stats;
}

Stats combat_get_modified_stats(Stats stats)
{
    int gun_damage = 0;
    switch(stats.gun)
    {
        case GUN_PISTOL:
        {
            gun_damage = 10;
        } break;
        case GUN_RIFLE:
        {
            gun_damage = 20;
        } break;
        case GUN_ROCKET:
        {
            gun_damage = 50;
        } break;
        default:
        {
            gun_damage = 0;
        } break;
    }

    int mod_dmg = stats.damage + gun_damage;
    if (mod_dmg > 100)
    {
        mod_dmg = 100;
    }

    stats.damage = mod_dmg;

    int armor_defense = 0;
    int armor_evasion = 0;
    switch(stats.armor)
    {
        case ARMOR_LEATHER:
        {
            armor_defense = 20;
            armor_evasion = 1;
        } break;
        case ARMOR_METAL:
        {
            armor_defense = 35;
            armor_evasion = 3;
        } break;
        case ARMOR_COMBAT:
        {
            armor_defense = 70;
            armor_evasion = 5;
        } break;
        default:
        {
            armor_defense = 0;
            armor_evasion = 0;
        } break;
    }

    int mod_def = stats.defense + armor_defense;
    if (mod_def > 100)
    {
        mod_def = 100;
    }

    stats.defense = mod_def;

    int mod_evasion = stats.evasion - armor_evasion;
    if (mod_evasion < 1)
    {
        mod_evasion = 1;
    }

    stats.evasion = mod_evasion;

    return stats;
}

AttackResult combat_attack(Stats *att, Stats *def)
{
    // ATTACKER STATS
    // --------------
    Stats mod_att = combat_get_modified_stats(*att);

    // DEFENDER STATS
    // --------------
    Stats mod_def = combat_get_modified_stats(*def);

    // CHANCE TO HIT CALC
    // ------------------
    int hit = rand() % 100;

    int chance_d = mod_att.accuracy - mod_def.evasion;

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

    // DAMAGE APPLICATION
    // ------------------

    int def_modifier = 100 - mod_def.defense;
    if (def_modifier < 5)
    {
        def_modifier = 5;
    }

    int randomized_dmg = rand() % (mod_att.damage / 3) - mod_att.damage / 6 + mod_att.damage;

    int end_dmg = (int) (dam_coef * (randomized_dmg * def_modifier * 0.01f));
    def->health -= end_dmg;

    // ATTACK RESULT
    // -------------

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
