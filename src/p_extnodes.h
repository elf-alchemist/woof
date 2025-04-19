//
//  Copyright (C) 1999 by
//  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
//  Copyright(C) 2015-2020 Fabian Greffrath
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
// DESCRIPTION:
//      support maps with NODES in uncompressed XNOD/XGLN or compressed
//      ZNOD/ZGLN formats, or DeePBSP format
//
//-----------------------------------------------------------------------------

#ifndef __P_EXTNODES__
#define __P_EXTNODES__

#include "doomtype.h"
#include "doomdata.h"

struct vertex_s;

typedef enum
{
    NFMT_DOOM,
    NFMT_DEEP,
    NFMT_XNOD,
    NFMT_ZNOD,
    NFMT_XGLN,
    NFMT_ZGLN,
    NFMT_XGL2,
    NFMT_ZGL2,
    NFMT_XGL3,
    NFMT_ZGL3,

    NFMT_UNSUPPORTED = NFMT_XGL2
} nodeformat_t;

const char * const node_format_names[] = {
  [NFMT_DOOM] = "Doom",
  [NFMT_DEEP] = "DeepBSP",
  [NFMT_XNOD] = "XNOD",
  [NFMT_ZNOD] = "ZNOD",
  [NFMT_XGLN] = "XGLN",
  [NFMT_ZGLN] = "ZGLN",
  [NFMT_XGL2] = "NanoBSP",
  [NFMT_ZGL2] = "NanoBSP",
  [NFMT_XGL3] = "NanoBSP",
  [NFMT_ZGL3] = "NanoBSP",
};

extern mapformat_t P_CheckMapFormat(int lumpnum);
extern nodeformat_t P_CheckNodeFormat(int lumpnum);
extern int P_GetOffset(struct vertex_s *v1, struct vertex_s *v2);

extern void P_LoadSegs_DEEP(int lump);
extern void P_LoadSubsectors_DEEP(int lump);
extern void P_LoadNodes_DEEP(int lump);
extern void P_LoadNodes_XNOD(int lump, boolean compressed, boolean glnodes);

#endif
