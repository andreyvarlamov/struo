#include "main.h"

void ui_printf(Glyph *ui, int ui_width, int ui_height, Point pos,
               const char* to_print, ...)
{
    char buffer[40];
    va_list args;
    va_start(args, to_print);
    vsnprintf(buffer, sizeof(buffer), to_print, args);
    va_end(args);

    for (size_t c_i = 0; buffer[c_i]; c_i++)
    {
        Point char_pos = { pos.x +c_i, pos.y};
        if (util_check_p_in_bounds(char_pos, ui_width, ui_height))
        {
            ui[util_p_to_i(char_pos, ui_width)] = buffer[c_i];
        }
        else
        {
            return;
        }
    }
}
