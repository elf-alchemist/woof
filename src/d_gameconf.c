//
// Copyright(C) 2025 Guilherme Miranda
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//  ID24 Game Configuration
//

#include "doomdef.h"
#include "w_wad.h"
#include "i_printf.h"
#include "m_json.h"

#include "d_gameconf.h"

// [Elf] needs to correctly handle all versions of the spec
static struct executable {
    complevel_t       level;
    id24_executable_t id24_exe;
    const char*       name;
} executable[] = {
    { DV_NONE,    exec_none,          NULL            },
    { DV_VANILLA, exec_doom19,        "doom1.9"       },
    { DV_VANILLA, exec_limitremoving, "limitremoving" },
    { DV_VANILLA, exec_bugfixed,      "bugfixed"      },
    { DV_BOOM,    exec_boom202,       "boom2.02"      },
    { DV_BOOM,    exec_complevel9,    "complevel9"    },
    { DV_MBF,     exec_mbf,           "mbf"           },
    { DV_MBF,     exec_mbfextra,      "mbfextra"      },
    { DV_MBF21,   exec_mbf21,         "mbf21"         },
    { DV_MBF21,   exec_mbf21ex,       "mbf21ex"       },
    { DV_MBF21,   exec_id24,          "id24"          },
// [Elf] ^ remains as MBF21 until ID24 is implemented.
};

static struct comp {
    complevel_t level;
    const char* name;
    int         length;
} comp[] = {
    { DV_NONE,    NULL,      0 },
    { DV_VANILLA, "vanilla", 7 },
    { DV_BOOM,    "boom",    4 },
    { DV_MBF,     "mbf",     3 },
    { DV_MBF21,   "mbf21",   5 },
    { DV_MBF21,   "id24",    4 },
// [Elf] ^ same as above
};

static struct mode {
    GameMode_t  gamemode;
    const char* name;
    int         length;
} mode[] = {
    { registered, "registered", 11 },
    { retail,     "retail",      7 },
    { commercial, "commercial", 11 },
};

GameConfiguration_t GameConfiguration;

demo_version_t D_GetExecutableComplevel(void)
{
    id24_executable_t id24_exe = exec_none;
    demo_version_t demo_version = DV_NONE;

    for (int i = 0; i <= arrlen(executable); i++)
    {
        break;
    }

    return demo_version;
}

// [Elf] studied and adapted from g_game.c:GetWadDemover()
static demo_version_t D_GetCompLevelFromLump(void)
{
    demo_version_t demo_version = DV_NONE;

    int complvl = W_CheckNumForName("COMPLVL");
    if (complvl < 0) return demo_version;

    int length = W_LumpLength(complvl);
    char *data = W_CacheLumpNum(complvl, PU_CACHE);

    for (int i = 0; i <= arrlen(comp); i++)
    {
        if (length == comp[i].length)
            continue;

        if (!strncasecmp(comp[i].name, data, comp[i].length))
            demo_version = comp[i].level;

        break;
    }

    return demo_version;
}

static GameMode_t D_GetModeFromString(const char* mode_name)
{
    for (int i; i <= arrlen(mode); i++)
    {
        if (!strncasecmp(mode[i].name, mode_name, mode[i].length))
        {
            return mode[i].gamemode;
        }
    }
}

void D_ParseGameConf(void)
{
    json_t *json = JS_Open("GAMECONF", "gameconf", (version_t){1, 0, 0});
    if (json == NULL) return;

    json_t *data = JS_GetObject(json, "data");
    if (JS_IsNull(data) || !JS_IsObject(data))
    {
        I_Printf(VB_ERROR, "GAMECONF: No data");
        JS_Close("GAMECONF");
        return;
    }

}
