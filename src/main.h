#ifndef MAIN_H
#define MAIN_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define global_variable static
#define local_persist static
#define internal static

#define MAP_COLS 65
#define MAP_ROWS 65
#define UI_COLS  40

#define SCREEN_COLS (MAP_COLS + UI_COLS)
#define SCREEN_ROWS (MAP_ROWS)

#define SCREEN_TILE_WIDTH 16.0f

// NOTE: Assumes square font in a square atlas
#define ATLAS_COLS 16.0f

#define ENTITY_NUM 50

#define ENEMY_FOV 15
#define PLAYER_FOV 15

typedef GLuint TexID;
typedef GLuint ShaderID;
typedef GLuint VaoID;
typedef GLuint VboID;

typedef unsigned char Glyph;

#include "util.c"
#include "resource.c"
#include "render.c"
#include "pathfinding.c"
#include "map.c"
#include "entity.c"
#include "combat.c"
#include "ui.c"
#include "game.c"

#endif
