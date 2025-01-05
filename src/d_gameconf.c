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
#include "d_gameconf.h"
#include "m_json.h"

// [Elf] gracefully handle all versions of the spec
struct {
    complevel_t       complevel;
    id24_executable_t id24_executable;
    const char*       name;
} gameconf_compat[] = {
    { CL_VANILLA, EXEC_DOOM19,         "doom1.9"       },
    { CL_VANILLA, EXEC_LIMITREMOVING,  "limitremoving" },
    { CL_VANILLA, EXEC_BUGFIXED,       "bugfixed"      },
    { CL_BOOM,    EXEC_BOOM202,        "boom2.02"      },
    { CL_BOOM,    EXEC_COMPLEVEL9,     "complevel9"    },
    { CL_MBF,     EXEC_MBF,            "mbf"           },
    { CL_MBF,     EXEC_MBFEXTRA,       "mbfextra"      },
    { CL_MBF21,   EXEC_MBF21,          "mbf21"         },
    { CL_MBF21,   EXEC_MBF21EX,        "mbf21ex"       },
    { CL_MBF21,   EXEC_ID24,           "id24"          },
// [Elf] ^ remains as MBF21 until CL_ID24 is implemented.
};

struct {
    GameMode_t  gamemode;
    const char* name;
} gameconf_mode[] = {
    { registered, "registered" },
    { retail,     "retail"     },
    { commercial, "commercial" },
};
