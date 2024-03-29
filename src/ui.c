#include "main.h"

global_variable int log_cursor = 0;

global_variable Point location_origin = { 1, UI_LOCATION_ROW };
global_variable Point stats_origin = { 1, UI_STATS_ROW };
global_variable Point items_origin = { 1, UI_ITEMS_ROW };
global_variable Point machine_origin = { 1, UI_MACHINE_ROW };
global_variable Point interact_origin = { 1, SCREEN_ROWS - LOG_LINES - 5 };
global_variable Point log_origin = { 1, SCREEN_ROWS - LOG_LINES - 1 };

void ui_reset_log_cursor()
{
    log_cursor = 0;
}

void ui_print(Glyph *ui, int ui_width, int ui_height, Point pos,
              const char *to_print)
{
    for (size_t c_i = 0; to_print[c_i]; c_i++)
    {
        Point char_pos = { pos.x +c_i, pos.y};
        if (util_check_p_in_bounds(char_pos, ui_width, ui_height))
        {
            ui[util_p_to_i(char_pos, ui_width)] = to_print[c_i];
        }
        else
        {
            return;
        }
    }
}

void ui_printf(Glyph *ui, int ui_width, int ui_height, Point pos,
               const char *to_print, ...)
{
    char buffer[40];
    va_list args;
    va_start(args, to_print);
    vsnprintf(buffer, sizeof(buffer), to_print, args);
    va_end(args);

    ui_print(ui, ui_width, ui_height, pos, buffer);
}

void ui_draw_header(Glyph *ui, int ui_width, int ui_height)
{
    Point p = { 15, 2 };
    ui_printf(ui, UI_COLS, SCREEN_ROWS, p, "%c %c struo", 0xE0, 0xEA);
}

void ui_draw_location(Glyph *ui, int ui_width, int ui_height, int current_level, int current_building)
{
    for (int i = 0; i < UI_COLS - 2; i++)
    {
        Point offset_p = { i, 2 };
        Point char_p = { location_origin.x + offset_p.x, location_origin.y + offset_p.y };
        ui[util_p_to_i(char_p, ui_width)] = 0;
    }

    ui_printf(ui, ui_width, ui_height, location_origin, " LOCATION ");

    Point p = { location_origin.x, location_origin.y + 2 };
    if (current_level == 999)
    {
        ui_printf(ui, ui_width, ui_height, p, "???");

    }
    else if (current_level == 0)
    {
        ui_printf(ui, ui_width, ui_height, p, "Computer Altar");
    }
    else
    {
        ui_printf(ui, ui_width, ui_height, p, "Building %04d. Floor %d", current_building, current_level);
    }
}

void ui_draw_log(Glyph *ui, int ui_width, int ui_height, char **log_lines)
{
    Point header_pos = { log_origin.x, log_origin.y - 2};
    ui_printf(ui, ui_width, ui_height, header_pos, " %cLog ", 0x1F);

    for (int i = 0; i < LOG_LINES * UI_COLS; i++)
    {
        Point offset_p = util_i_to_p(i, ui_width);
        Point char_p = { log_origin.x + offset_p.x, log_origin.y + offset_p.y };
        ui[util_p_to_i(char_p, ui_width)] = 0;
    }

    for (int i = 0; i < LOG_LINES; i++)
    {
        // int cursor = (log_cursor - i) % LOG_LINES;

        int cursor = (LOG_LINES + log_cursor - i) % LOG_LINES;

        if (log_lines[cursor])
        {
            Point line_pos = { log_origin.x, ui_height - 2 - i};
            // ui_printf(ui, ui_width, ui_height, line_pos, "%d. %s", cursor, log_lines[cursor]);
            ui_printf(ui, ui_width, ui_height, line_pos, log_lines[cursor]);
        }
    }
}

void ui_add_log_line(Glyph *ui, int ui_width,  int ui_height,
                     char **log_lines, const char *line, ...)
{
    char buffer[40];
    va_list args;
    va_start(args, line);
    vsnprintf(buffer, sizeof(buffer), line, args);
    va_end(args);

    log_cursor = (log_cursor + 1) % LOG_LINES;
    strcpy(log_lines[log_cursor], buffer);

    ui_draw_log(ui, ui_width, ui_height, log_lines);
}

void ui_draw_player_stats(Glyph *ui, int ui_width, int ui_height, Stats stats)
{
    Stats mod_stats = combat_get_modified_stats(stats);
    for (int i = 0; i < 11 * ui_width; i++)
    {
        Point offset_p = util_i_to_p(i, ui_width);
        Point char_p = { stats_origin.x + offset_p.x, stats_origin.y + offset_p.y };
        ui[util_p_to_i(char_p, ui_width)] = 0;
    }

    ui_printf(ui, ui_width, ui_height, stats_origin, " STATS ");

    Point health_pos = { stats_origin.x, stats_origin.y + 2 };
    ui_printf(ui, ui_width, ui_height, health_pos, "HP:   %d/%d", mod_stats.health, mod_stats.max_health);

    Point damage_pos = { stats_origin.x, stats_origin.y + 3 };
    ui_printf(ui, ui_width, ui_height, damage_pos, "DMG:  %d", mod_stats.damage);

    Point defense_pos = { stats_origin.x, stats_origin.y + 4 };
    ui_printf(ui, ui_width, ui_height, defense_pos, "DEF:  %d%%", mod_stats.defense);

    Point accuracy_pos = { stats_origin.x, stats_origin.y + 5 };
    ui_printf(ui, ui_width, ui_height, accuracy_pos, "ACC:  %d/10", mod_stats.accuracy);

    Point evasion_pos = { stats_origin.x, stats_origin.y + 6 };
    ui_printf(ui, ui_width, ui_height, evasion_pos, "EVAS: %d/10", mod_stats.evasion);

    Point speed_pos = { stats_origin.x, stats_origin.y + 7 };
    ui_printf(ui, ui_width, ui_height, speed_pos, "SPD:  %d", mod_stats.speed);

    Point armor_pos = { stats_origin.x, stats_origin.y + 9 };
    AString armor_name = item_get_armor_name(stats.armor);
    ui_printf(ui, ui_width, ui_height, armor_pos, "ARMOR: %s", armor_name.str);

    Point gun_pos = { stats_origin.x, stats_origin.y + 10 };
    AString gun_name = item_get_gun_name(stats.gun);
    ui_printf(ui, ui_width, ui_height, gun_pos, "WEAPN: %s", gun_name.str);
}

void ui_clean_interact(Glyph *ui, int ui_width, int ui_height)
{
    for (int i = 0; i < UI_COLS - 1; i++)
    {
        Point offset_p = { i, 0 };
        Point char_p = { interact_origin.x + offset_p.x, interact_origin.y + offset_p.y };
        ui[util_p_to_i(char_p, ui_width)] = 0;
    }
}

void ui_draw_interact_item(Glyph *ui, int ui_width, int ui_height, ItemType item_type)
{
    ui_clean_interact(ui, ui_width, ui_height);
    AString name = item_get_item_name(item_type);
    ui_printf(ui, ui_width, ui_height, interact_origin, "G - pick up %s.", name.str);
}

void ui_draw_interact_exit(Glyph *ui, int ui_width, int ui_height, int to_level)
{
    ui_clean_interact(ui, ui_width, ui_height);
    if (to_level == 1)
    {
        ui_printf(ui, ui_width, ui_height, interact_origin,
                  "G - scavenge for computer parts.");
    }
    else if (to_level == 0)
    {
        ui_printf(ui, ui_width, ui_height, interact_origin,
                  "G - back to the computer altar.");
    }
    else
    {
        ui_printf(ui, ui_width, ui_height, interact_origin,
                  "G - exit to floor %d.", to_level);
    }
}

void ui_draw_interact_machine_plan(Glyph *ui, int ui_width, int ui_height, MachineType machine_type)
{
    ui_clean_interact(ui, ui_width, ui_height);
    AString name = entity_get_machine_name(machine_type);
    ui_printf(ui, ui_width, ui_height, interact_origin, "G - assemble %s.", name.str);
}

void ui_draw_interact_built_machine(Glyph *ui, int ui_width, int ui_height, MachineType machine_type)
{
    ui_clean_interact(ui, ui_width, ui_height);
    if (machine_type != MACHINE_COMPUTER)
    {
        ItemType craftable = item_machine_to_item_it_crafts(machine_type);
        AString name = item_get_item_name(craftable);
        ui_printf(ui, ui_width, ui_height, interact_origin, "G - craft %s.", name.str);
    }
    else
    {
        ui_printf(ui, ui_width, ui_height, interact_origin, "G - execute the nuclear sequence.");
    }
}

void ui_draw_player_items(Glyph *ui, int ui_width, int ui_height, int *item_counts)
{
    int rows = ITEM_MAX - ITEM_MECH_COMP + 2;
    int row_i = 0;

    for (int i = 0; i < rows * ui_width; i++)
    {
        Point offset_p = util_i_to_p(i, ui_width);
        Point char_p = { items_origin.x + offset_p.x, items_origin.y + offset_p.y };
        ui[util_p_to_i(char_p, ui_width)] = 0;
    }

    ui_printf(ui, ui_width, ui_height, items_origin, " ITEMS ");

    for (int i = ITEM_MECH_COMP; i < ITEM_MAX; i++)
    {
        if (item_counts[i] > 0 || i < ITEM_CPU_AUTOMAT_FRAME)
        {
            Point pos = { items_origin.x, items_origin.y + row_i + 2 };
            AString item_name = item_get_item_name(i);
            ui_printf(ui, ui_width, ui_height, pos, "%d - %s", item_counts[i], item_name.str);
            row_i++;
        }
    }
}

void ui_clean_machine(Glyph *ui, int ui_width, int ui_height)
{
    for (int i = 0; i < 7 * ui_width; i++)
    {
        Point offset_p = util_i_to_p(i, ui_width);
        Point char_p = { machine_origin.x + offset_p.x - 1, machine_origin.y + offset_p.y };
        ui[util_p_to_i(char_p, ui_width)] = 0;
    }
}

void ui_draw_machine(Glyph *ui, int ui_width, int ui_height, MachineType machine_type, int *item_counts, bool built)
{
    ui_clean_machine(ui, ui_width, ui_height);

    for (int i = 0; i < ui_width; i++)
    {
        ui[util_xy_to_i(i, machine_origin.y, ui_width)] = 0xC4;
    }
    ui[util_xy_to_i(0, machine_origin.y, ui_width)] = 0xC3;
    ui[util_xy_to_i(ui_width - 1, machine_origin.y, ui_width)] = 0xB4;

    if (!built)
    {
        ui_printf(ui, ui_width, ui_height, machine_origin, " MACHINE PLAN: %s ", entity_get_machine_name(machine_type).str);

        switch (machine_type)
        {
            case MACHINE_CPU_AUTOMATON:
            case MACHINE_MOBO_AUTOMATON:
            case MACHINE_GPU_AUTOMATON:
            case MACHINE_MEM_AUTOMATON:
            case MACHINE_ASSEMBLER:
            case MACHINE_COMPUTER:
            {
                CanCraftResult ccr = item_can_craft(machine_type, ITEM_NONE, item_counts);

                int print_offset = 2;

                for (ItemType type = ITEM_MECH_COMP; type < ITEM_MAX; type++)
                {
                    if (ccr.need_items[type] > 0)
                    {
                        Point pos = { machine_origin.x, machine_origin.y + print_offset };
                        AString item_name = item_get_item_name(type);
                        ui_printf(ui, ui_width, ui_height, pos,
                                  "%02d/%02d - %s", item_counts[type], ccr.need_items[type], item_name.str);

                        print_offset++;
                    }
                }
            } break;

            default:
            {

            } break;
        }
    }
    else
    {
        ui_printf(ui, ui_width, ui_height, machine_origin, " MACHINE: %s ", entity_get_machine_name(machine_type).str);

        switch (machine_type)
        {
            case MACHINE_CPU_AUTOMATON:
            case MACHINE_MOBO_AUTOMATON:
            case MACHINE_GPU_AUTOMATON:
            case MACHINE_MEM_AUTOMATON:
            case MACHINE_ASSEMBLER:
            {
                ItemType craftable = item_machine_to_item_it_crafts(machine_type);
                CanCraftResult ccr = item_can_craft(MACHINE_NONE, craftable, item_counts);

                int print_offset = 2;

                for (ItemType type = ITEM_MECH_COMP; type < ITEM_MAX; type++)
                {
                    if (ccr.need_items[type] > 0)
                    {
                        Point pos = { machine_origin.x, machine_origin.y + print_offset };
                        AString item_name = item_get_item_name(type);
                        ui_printf(ui, ui_width, ui_height, pos,
                                  "%02d/%02d - %s", item_counts[type], ccr.need_items[type], item_name.str);

                        print_offset++;
                    }
                }
            } break;

            case MACHINE_COMPUTER:
            {
                Point pos = util_xy_to_p(machine_origin.x, machine_origin.y + 2);
                ui_printf(ui, ui_width, ui_height, pos, "%c%c%c%c%c%c%c%c%c%cshes to ashes", 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92);
            }

            default:
            {

            } break;
        }
    }

}
