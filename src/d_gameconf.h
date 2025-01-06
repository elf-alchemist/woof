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

#include "doomstat.h"
#include "g_game.h"
#include "m_json.h"

#ifndef D_GAMECONF
#define D_GAMECONF

// [Elf] mismatched with compability systems outside of R&R/KEX Doom
typedef enum id24_executable_t {
    exec_none = -1,
    exec_doom19,
    exec_limitremoving,
    exec_bugfixed,
    exec_boom202,
    exec_complevel9,
    exec_mbf,
    exec_mbfextra,
    exec_mbf21,
    exec_mbf21ex,
    exec_id24,
} id24_executable_t;

typedef struct GameConfiguration_t {
    const char* title;
    const char* version;
    const char* author;
    const char* description;

    const char*  iwad;
    const char** pwads;
    const char** pwadfiles;    // 0.99.1
    const char** dehfiles;     // 0.99.1

    const char* executable;
    const char* complevel;     // COMPLVL
    const char* mode;
    const char* options;       // OPTIONS

    const char** player_xlats;
    const char** wad_xlats;
} GameConfiguration_t;

#endif // D_GAMECONF
