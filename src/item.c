#include "main.h"

typedef enum ItemType
{
    ITEM_NONE,

    ITEM_HEALTH,
    ITEM_ARMOR_LEATHER,
    ITEM_ARMOR_METAL,
    ITEM_ARMOR_COMBAT,
    ITEM_GUN_PISTOL,
    ITEM_GUN_RIFLE,
    ITEM_GUN_ROCKET,

    ITEM_MECH_COMP,
    ITEM_ELEC_COMP,
    ITEM_JUNK,

    ITEM_CPU_AUTOMAT_FRAME,
    ITEM_MOBO_AUTOMAT_FRAME,
    ITEM_GPU_AUTOMAT_FRAME,
    ITEM_MEM_AUTOMAT_FRAME,
    ITEM_ASSEMBLER_FRAME,

    ITEM_MAX
} ItemType;

typedef enum ArmorType
{
    ARMOR_NONE,

    ARMOR_LEATHER,
    ARMOR_METAL,
    ARMOR_COMBAT,

    ARMOR_MAX
} ArmorType;

typedef enum GunType
{
    GUN_NONE,

    GUN_PISTOL,
    GUN_RIFLE,
    GUN_ROCKET,

    GUN_MAX
} GunType;

AString item_get_item_name(ItemType item_type)
{
    AString name = {0};
    switch (item_type)
    {
        case ITEM_HEALTH:
        {
            strcpy(name.str, "Health Pack");
        } break;

        case ITEM_ARMOR_LEATHER:
        {
            strcpy(name.str, "Leather Armor");
        } break;

        case ITEM_ARMOR_METAL:
        {
            strcpy(name.str, "Metal Armor");
        } break;

        case ITEM_ARMOR_COMBAT:
        {
            strcpy(name.str, "Combat Armor");
        } break;

        case ITEM_GUN_PISTOL:
        {
            strcpy(name.str, "Pistol");
        } break;

        case ITEM_GUN_RIFLE:
        {
            strcpy(name.str, "Assault Rifle");
        } break;

        case ITEM_GUN_ROCKET:
        {
            strcpy(name.str, "Rocket Launer");
        } break;

        case ITEM_MECH_COMP:
        {
            strcpy(name.str, "Mechanical Comp");
        } break;

        case ITEM_ELEC_COMP:
        {
            strcpy(name.str, "Electrical Comp");
        } break;

        case ITEM_JUNK:
        {
            strcpy(name.str, "Junk");
        } break;

        case ITEM_CPU_AUTOMAT_FRAME:
        {
            strcpy(name.str, "CPU AutomatFrame");
        } break;

        case ITEM_MOBO_AUTOMAT_FRAME:
        {
            strcpy(name.str, "MoBo AutomatFrame");
        } break;

        case ITEM_GPU_AUTOMAT_FRAME:
        {
            strcpy(name.str, "GPU AutomatFrame");
        } break;

        case ITEM_MEM_AUTOMAT_FRAME:
        {
            strcpy(name.str, "MEM AutomatFrame");
        } break;

        case ITEM_ASSEMBLER_FRAME:
        {
            strcpy(name.str, "Assembler Frame");
        } break;

        default:
        {
            strcpy(name.str, "None");
        }
    }

    return name;
}

AString item_get_armor_name(ArmorType armor_type)
{
    AString name = {0};
    switch (armor_type)
    {
        case ARMOR_LEATHER:
        {
            strcpy(name.str, "Leather");
        } break;

        case ARMOR_METAL:
        {
            strcpy(name.str, "Metal");
        } break;

        case ARMOR_COMBAT:
        {
            strcpy(name.str, "Combat");
        } break;

        default:
        {
            strcpy(name.str, "None");
        }
    }

    return name;
}

AString item_get_gun_name(GunType gun_type)
{
    AString name = {0};
    switch (gun_type)
    {
        case GUN_PISTOL:
        {
            strcpy(name.str, "Pistol");
        } break;

        case GUN_RIFLE:
        {
            strcpy(name.str, "Assault Rifle");
        } break;

        case GUN_ROCKET:
        {
            strcpy(name.str, "Rocket Launcher");
        } break;

        default:
        {
            strcpy(name.str, "None");
        }
    }

    return name;
}
