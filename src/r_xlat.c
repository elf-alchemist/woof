//
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

#include "r_xlat.h"
#include "i_printf.h"
#include "i_video.h"
#include "m_json.h"
#include "r_state.h"
#include "v_patch.h"
#include "v_trans.h"
#include "w_wad.h"
#include "z_zone.h"
#include <string.h>

size_t R_XlatIndexByName(const char *name)
{
    for (const xlat_t *p = xlats; p->name; ++p)
    {
        if (!strcmp(p->name, name))
        {
            return (p - xlats);
        }
    }
    return 0;
}

const byte *R_XlatTableByName(const char *name)
{
    size_t i = R_XlatIndexByName(name);
    return xlats[i].table;
}

const byte *R_XlatTableByCR(crange_idx_e cr_index)
{
    size_t i = 0;
    return xlats[i].table;
}

//
// Palette translation support
//
xlat_t *xlats = NULL;

byte v_lightest_color = 0;
byte v_darkest_color = 0;
byte invul_gray[256] = {0};

// [FG] dark/shaded color translation table
byte *xlat_dark;
byte *xlat_shaded;
byte *xlat_bright;

// jff 4/24/98 initialize this at runtime
byte *colrngs[CR_LIMIT] = {0};
byte *red2col[CR_LIMIT] = {0};

// [FG] translate between blood color value as per EE spec
//      and actual color translation table index

static const int32_t bloodcolor[] = {
    CR_RED,    // 0 - Red (normal)
    CR_GRAY,   // 1 - Grey
    CR_GREEN,  // 2 - Green
    CR_BLUE2,  // 3 - Blue
    CR_YELLOW, // 4 - Yellow
    CR_BLACK,  // 5 - Black
    CR_PURPLE, // 6 - Purple
    CR_WHITE,  // 7 - White
    CR_ORANGE, // 8 - Orange
};

int32_t V_BloodColor(int32_t blood)
{
    return bloodcolor[blood];
}

//
// Vanilla-style range-based table
//
static inline void BuildTranslationTable(xlat_t *xlat, byte src_min,
                                         byte src_max, byte target_min,
                                         byte target_max)
{
    const fixed_t xlat_step = FixedDiv(IntToFixed(target_max - target_min + 1),
                                       IntToFixed(src_max - src_min + 1));
    for (int32_t index = 0; index <= 256; index++)
    {
        int32_t value = index;
        if (index >= src_min && index <= src_max)
        {
            fixed_t target_increment = xlat_step * (index - src_min);
            value = target_min + FixedToInt(target_increment);
        }
        xlat->table[index] = value;
    }
}

//
// Original vanilla tables + ID24 additions
//
const static struct
{
    const char *const mnemonic;
    const char *const sbar_back;
    const char *const inter_back;
    const byte src_min;
    const byte src_max;
    const byte target_min;
    const byte target_max;
} player_xlats[] = {
    {"ID24_COLOR_GREEN",   "STFB0", "STPB0", 0xFF, 0x00, 0xFF, 0x00},
    {"ID24_COLOR_INDIGO",  "STFB1", "STPB1", 0x70, 0x7F, 0x60, 0x6F},
    {"ID24_COLOR_BROWN",   "STFB2", "STPB2", 0x70, 0x7F, 0x40, 0x4F},
    {"ID24_COLOR_RED",     "STFB3", "STPB3", 0x70, 0x7F, 0x20, 0x2F},
    {"ID24_COLOR_YELLOW",  "STFB0", "STPB0", 0x70, 0x7F, 0xA0, 0xA7},
    {"ID24_COLOR_BLUE",    "STFB0", "STPB0", 0x70, 0x7F, 0xC4, 0xCF},
    {"ID24_COLOR_NAVY",    "STFB0", "STPB0", 0x70, 0x7F, 0xF0, 0xF7},
    {"ID24_COLOR_MAGENTA", "STFB0", "STPB0", 0x70, 0x7F, 0xFA, 0xFF},
};

const static int32_t num_player_xlats = arrlen(player_xlats);

//
// Load built-in vanilla/boom tables
//
static void LoadBuiltInTables(void)
{
}

//
// ID24 translation json lump
//
xlat_t *R_LoadTranslation(const char *lumpname)
{
    json_t *json = JS_Open(lumpname, "translation", (version_t){1, 0, 0});
    if (json == NULL)
    {
        return NULL;
    }

    json_t *data = JS_GetObject(json, "data");
    if (JS_IsNull(data) || !JS_IsObject(data))
    {
        I_Printf(VB_WARNING, "TRANSLATION: data object not defined");
        JS_Close(lumpname);
        return NULL;
    }

    const char *name = JS_GetStringValue(data, "name");
    const char *sbar_back = JS_GetStringValue(data, "sbarback");
    const char *inter_back = JS_GetStringValue(data, "interback");
    const boolean sbar_back_translate =
        JS_GetBooleanValue(data, "sbartranslate");
    const boolean inter_back_translate =
        JS_GetBooleanValue(data, "intertranslate");
    json_t *table = JS_GetObject(data, "table");

    if (JS_GetArraySize(table) != 256)
    {
        I_Printf(VB_WARNING,
                 "TRANSLATION: table of incorrect size on lump: '%s'",
                 lumpname);
        JS_Close(lumpname);
        return NULL;
    }

    xlat_t *out = Z_Malloc(sizeof(xlat_t), PU_STATIC, NULL);
    out->name = Z_StrDup(name, PU_STATIC);
    out->lumpname = Z_StrDup(lumpname, PU_STATIC);

    int32_t sbar_back_index = W_CheckNumForName(sbar_back);
    out->sbar_back = (sbar_back_index >= 0)
                         ? V_CachePatchNum(sbar_back_index, PU_STATIC)
                         : NULL;
    out->sbar_translate = sbar_back_translate;

    int32_t inter_back_index = W_CheckNumForName(inter_back);
    out->inter_back = (inter_back_index >= 0)
                          ? V_CachePatchNum(inter_back_index, PU_STATIC)
                          : NULL;
    out->inter_translate = inter_back_translate;

    for (int32_t i = 0; i < 256; i++)
    {
        out->table[i] = JS_GetInteger(JS_GetArrayItem(table, i));
    }

    return out;
}

//
// Actual initialization starts here
//

void R_InitTranslations(void)
{
    byte *playpal = W_CacheLumpName("PLAYPAL", PU_STATIC);
    byte *palsrc = playpal;

    xlat_bright = malloc(256);
    for (int i = 0; i < 256; ++i)
    {
        xlat_bright[i] = V_Colorize(playpal, CR_BRIGHT, (byte)i);
    }

    // [FG] dark/shaded color translation table
    xlat_dark = &colormaps[0][256 * 15];
    xlat_shaded = &colormaps[0][256 * 6];

    v_lightest_color = I_GetNearestColor(playpal, 0xFF, 0xFF, 0xFF);
    v_darkest_color = I_GetNearestColor(playpal, 0x00, 0x00, 0x00);

    for (int i = 0; i < 256; ++i)
    {
        double red = *palsrc++ / 256.0;
        double green = *palsrc++ / 256.0;
        double blue = *palsrc++ / 256.0;

        // formula is taken from dcolors.c preseving "Carmack's typo"
        // https://doomwiki.org/wiki/Carmack%27s_typo
        int gray = (red * 0.299 + green * 0.587 + blue * 0.144) * 255;
        invul_gray[i] = I_GetNearestColor(playpal, gray, gray, gray);
    }
}
