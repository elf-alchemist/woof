//
//  Copyright(C) 2020 Ethan Watson
//  Copyright (C) 2026 Guilherme Miranda
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  DESCRIPTION:
//    ID24 Palette Translation system
//

#include "doomtype.h"
#include "r_defs.h"
#include <stddef.h>

//
// Palette translations
//

typedef byte xlat_data[256];

typedef struct xlat_s
{
    const char *lumpname;
    const char *name;
    const patch_t *sbar_back;
    const patch_t *inter_back;
    boolean sbar_translate;
    boolean inter_translate;
    xlat_data table;
} xlat_t;

// symbolic indices into color translation table pointer array
typedef enum
{
    CR_ORIG = -1,
    CR_BRICK,  // 0
    CR_TAN,    // 1
    CR_GRAY,   // 2
    CR_GREEN,  // 3
    CR_BROWN,  // 4
    CR_GOLD,   // 5
    CR_RED,    // 6
    CR_BLUE1,  // 7
    CR_ORANGE, // 8
    CR_YELLOW, // 9
    CR_BLUE2,  // 10
    CR_BLACK,  // 11
    CR_PURPLE, // 12
    CR_WHITE,  // 13
    CR_NONE,   // 14 // [FG] dummy
    CR_BRIGHT, // 15
    CR_LIMIT   // 16 //jff 2/27/98 added for range check
} crange_idx_e;

//
// Palette translation support
//
extern xlat_t *xlats;
typedef size_t xlat_index;

extern byte v_lightest_color;
extern byte v_darkest_color;
extern byte invul_gray[256];

// [FG] dark/shaded color translation table
extern byte *xlat_dark;
extern byte *xlat_shaded;
extern byte *xlat_bright;

extern byte invul_gray[];

// array of pointers to color translation tables
extern byte *colrngs[];
extern byte *red2col[];

#define ORIG_S  "\x1b\x2f"
#define BRICK_S "\x1b\x30"
#define TAN_S   "\x1b\x31"
#define GRAY_S  "\x1b\x32"
#define GREEN_S "\x1b\x33"
#define BROWN_S "\x1b\x34"
#define GOLD_S  "\x1b\x35"
#define RED_S   "\x1b\x36"
#define BLUE1_S "\x1b\x37"
#define BLUE2_S "\x1b\x3a"

size_t R_XlatIndexByName(const char *name);
const byte *R_XlatTableByName(const char *name);
const byte *R_XlatTableByCR(crange_idx_e index);
void R_InitTranslations(void);
