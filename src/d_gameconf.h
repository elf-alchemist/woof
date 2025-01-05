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

// [Elf] mismatched with compability systems outside of RNR/KEX Doom
typedef enum {
    EXEC_NONE = -1,
    EXEC_DOOM19,
    EXEC_LIMITREMOVING,
    EXEC_BUGFIXED,
    EXEC_BOOM202,
    EXEC_COMPLEVEL9,
    EXEC_MBF,
    EXEC_MBFEXTRA,
    EXEC_MBF21,
    EXEC_MBF21EX,
    EXEC_ID24,
} id24_executable_t;


#endif // D_GAMECONF
