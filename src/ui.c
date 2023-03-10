#include "main.h"

global_variable int log_cursor = 0;
global_variable Point log_origin = { 1, SCREEN_ROWS - LOG_LINES - 1 };

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
            Point line_pos = { log_origin.x, log_origin.y + i};
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
