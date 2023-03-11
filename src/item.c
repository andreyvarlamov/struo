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

    ITEM_CPU,
    ITEM_MOBO,
    ITEM_GPU,
    ITEM_MEM,

    ITEM_COMPUTER,

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
            strcpy(name.str, "Rocket Launcher");
        } break;

        case ITEM_MECH_COMP:
        {
            strcpy(name.str, "Mechanical Components");
        } break;

        case ITEM_ELEC_COMP:
        {
            strcpy(name.str, "Electrical Components");
        } break;

        case ITEM_JUNK:
        {
            strcpy(name.str, "Junk");
        } break;

        case ITEM_CPU_AUTOMAT_FRAME:
        {
            strcpy(name.str, "CPU Automaton Frame");
        } break;

        case ITEM_MOBO_AUTOMAT_FRAME:
        {
            strcpy(name.str, "MoBo Automaton Frame");
        } break;

        case ITEM_GPU_AUTOMAT_FRAME:
        {
            strcpy(name.str, "GPU Automaton Frame");
        } break;

        case ITEM_MEM_AUTOMAT_FRAME:
        {
            strcpy(name.str, "MEM Automaton Frame");
        } break;

        case ITEM_ASSEMBLER_FRAME:
        {
            strcpy(name.str, "Assembler Frame");
        } break;

        case ITEM_CPU:
        {
            strcpy(name.str, "CPU");
        } break;

        case ITEM_MOBO:
        {
            strcpy(name.str, "Motherboard");
        } break;

        case ITEM_GPU:
        {
            strcpy(name.str, "GPU");
        } break;

        case ITEM_MEM:
        {
            strcpy(name.str, "Memory");
        } break;

        case ITEM_COMPUTER:
        {
            strcpy(name.str, "Computer");
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

int item_get_comp_req(MachineType for_machine_type, ItemType for_item_type, ItemType req_type)
{
    int mech_req = 0;
    int elec_req = 0;
    int junk_req = 0;

    switch (for_machine_type)
    {
        case MACHINE_CPU_AUTOMATON:
        {
            mech_req = 4;
            elec_req = 4;
            junk_req = 4;
        } break;

        case MACHINE_MOBO_AUTOMATON:
        {
            mech_req = 5;
            elec_req = 3;
            junk_req = 4;
        } break;

        case MACHINE_GPU_AUTOMATON:
        {
            mech_req = 3;
            elec_req = 5;
            junk_req = 4;
        } break;

        case MACHINE_MEM_AUTOMATON:
        {
            mech_req = 4;
            elec_req = 3;
            junk_req = 5;
        } break;

        case MACHINE_ASSEMBLER:
        {
            mech_req = 5;
            elec_req = 5;
            junk_req = 5;
        } break;

        default:
        {

        } break;
    }

    switch (for_item_type)
    {
        case ITEM_CPU:
        {
            mech_req = 3;
            elec_req = 3;
            junk_req = 3;
        } break;

        case ITEM_MOBO:
        {
            mech_req = 3;
            elec_req = 5;
            junk_req = 1;
        } break;

        case ITEM_GPU:
        {
            mech_req = 4;
            elec_req = 4;
            junk_req = 1;
        } break;

        case ITEM_MEM:
        {
            mech_req = 2;
            elec_req = 5;
            junk_req = 2;
        } break;

        default:
        {

        } break;
    }

    switch (req_type)
    {
        case ITEM_MECH_COMP:
        {
            return mech_req;
        } break;

        case ITEM_ELEC_COMP:
        {
            return elec_req;
        } break;

        case ITEM_JUNK:
        {
            return junk_req;
        } break;

        default:
        {
            return -1;
        } break;
    }
}

typedef struct CanCraftResult
{
    int need_items[ITEM_MAX];
    bool yes;
} CanCraftResult;

CanCraftResult item_can_craft(MachineType for_machine_type, ItemType for_item_type, int *item_counts)
{
    CanCraftResult result = {0};

    if (for_machine_type == MACHINE_COMPUTER)
    {
        result.need_items[ITEM_COMPUTER] = 1;

        result.yes = item_counts[ITEM_COMPUTER] >= 1;
    }
    else if (for_item_type == ITEM_COMPUTER)
    {
        result.need_items[ITEM_CPU] = 1;
        result.need_items[ITEM_MOBO] = 1;
        result.need_items[ITEM_GPU] = 1;
        result.need_items[ITEM_MEM] = 1;

        result.yes = item_counts[ITEM_CPU]  >= 1
                  && item_counts[ITEM_MOBO] >= 1
                  && item_counts[ITEM_GPU]  >= 1
                  && item_counts[ITEM_MEM]  >= 1;
    }
    else
    {
        int mech_req = item_get_comp_req(for_machine_type, for_item_type, ITEM_MECH_COMP);
        int elec_req = item_get_comp_req(for_machine_type, for_item_type, ITEM_ELEC_COMP);
        int junk_req = item_get_comp_req(for_machine_type, for_item_type, ITEM_JUNK);

        bool have_or_dont_need_frame = true;

        if (for_machine_type == MACHINE_CPU_AUTOMATON)
        {
            result.need_items[ITEM_CPU_AUTOMAT_FRAME] = 1;
            have_or_dont_need_frame = item_counts[ITEM_CPU_AUTOMAT_FRAME] >= 1;
        }
        else if (for_machine_type == MACHINE_MOBO_AUTOMATON)
        {
            result.need_items[ITEM_MOBO_AUTOMAT_FRAME] = 1;
            have_or_dont_need_frame = item_counts[ITEM_MOBO_AUTOMAT_FRAME] >= 1;
        }
        else if (for_machine_type == MACHINE_GPU_AUTOMATON)
        {
            result.need_items[ITEM_GPU_AUTOMAT_FRAME] = 1;
            have_or_dont_need_frame = item_counts[ITEM_GPU_AUTOMAT_FRAME] >= 1;
        }
        else if (for_machine_type == MACHINE_MEM_AUTOMATON)
        {
            result.need_items[ITEM_MEM_AUTOMAT_FRAME] = 1;
            have_or_dont_need_frame = item_counts[ITEM_MEM_AUTOMAT_FRAME] >= 1;
        }
        else if (for_machine_type == MACHINE_ASSEMBLER)
        {
            result.need_items[ITEM_ASSEMBLER_FRAME] = 1;
            have_or_dont_need_frame = item_counts[ITEM_ASSEMBLER_FRAME] >= 1;
        }

        result.need_items[ITEM_MECH_COMP] = mech_req;
        result.need_items[ITEM_ELEC_COMP] = elec_req;
        result.need_items[ITEM_JUNK] = junk_req;


        result.yes = item_counts[ITEM_MECH_COMP] >= mech_req
                  && item_counts[ITEM_ELEC_COMP] >= elec_req
                  && item_counts[ITEM_JUNK]      >= junk_req
                  && have_or_dont_need_frame;
    }

    return result;
}

ItemType item_machine_to_item_it_crafts(MachineType machine_type)
{
    ItemType item_type = ITEM_NONE;

    switch (machine_type)
    {
        case MACHINE_CPU_AUTOMATON:
        {
            item_type = ITEM_CPU;
        } break;

        case MACHINE_MOBO_AUTOMATON:
        {
            item_type = ITEM_MOBO;
        } break;

        case MACHINE_GPU_AUTOMATON:
        {
            item_type = ITEM_GPU;
        } break;

        case MACHINE_MEM_AUTOMATON:
        {
            item_type = ITEM_MEM;
        } break;

        case MACHINE_ASSEMBLER:
        {
            item_type = ITEM_COMPUTER;
        } break;

        default:
        {

        } break;
    }

    return item_type;
}
