#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define global_variable static
#define local_persist static
#define internal static

#define SCREEN_COLS 40
#define SCREEN_ROWS 30
#define SCREEN_TILE_WIDTH 24.0f

// NOTE: Assumes square font in a square atlas
#define ATLAS_COLS 16.0f

#define ENTITY_INIT_COUNT 16

typedef GLuint TexID;
typedef GLuint ShaderID;
typedef GLuint VaoID;
typedef GLuint VboID;

typedef unsigned char Glyph;

#include "util.c"
#include "resource.c"
#include "render.c"
#include "map.c"
#include "ecs.c"
#include "game.c"

#endif