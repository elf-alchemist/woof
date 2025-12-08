//
//  Copyright (C) 1999 by
//   id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
//  Copyright (C) 2023 by
//   Ryan Krafnick
//  Copyright (C) 2025 by
//   Guilherme Miranda
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
//   Generation of transparency lookup tables.
//

#include "doomdef.h"
#include "doomstat.h"
#include "doomtype.h"
#include "i_video.h"
#include "m_argv.h"
#include "r_srgb.h"
#include "r_tranmap.h"
#include "w_wad.h"
#include "z_zone.h"

//
// R_InitTranMap
//
// Initialize translucency filter map
//
// By Lee Killough 2/21/98
//

static byte *normal_tranmap[ALPHA_MAX];

const byte *tranmap;      // translucency filter maps 256x256   // phares
const byte *main_tranmap; // killough 4/11/98
const byte *main_addimap; // Some things look better with added luminosity :)

//
// Blending algorthims!
//

enum
{
    r,
    g,
    b
};

//
// The heart of the calculation, the blending algorithm. Currently supported:
// * Normal -- applies standard alpha interpolation
// * Additive -- alpha is a foreground multiplier, added to unmodified background
//
// TODO, tentative additions:
// * Subtractive -- alpha is a foreground multiplier, subtracted from unmodifed background
//

typedef const byte (blendfunc_t)(const byte, const byte, const double);

inline static const byte BlendChannelNormal(const byte bg, const byte fg, const double a)
{
    const double fg_linear = byte_to_linear(fg);
    const double bg_linear = byte_to_linear(bg);
    const double r_linear = (fg_linear * a) + (bg_linear * (1.0 - a));
    return linear_to_byte(r_linear);
}

inline static const byte BlendChannelAdditive(const byte bg, const byte fg, const double a)
{
    const double fg_linear = byte_to_linear(fg);
    const double bg_linear = byte_to_linear(bg);
    const double r_linear = (fg_linear * a) + bg_linear;
    return linear_to_byte(r_linear);
}

inline static const byte ColorBlend(byte *playpal, blendfunc_t blendfunc,
                                    const byte *bg, const byte *fg,
                                    const double alpha)
{
    int blend[3] = {0};
    blend[r] = blendfunc(bg[r], fg[r], alpha);
    blend[g] = blendfunc(bg[g], fg[g], alpha);
    blend[b] = blendfunc(bg[b], fg[b], alpha);
    return I_GetNearestColor(playpal, blend[r], blend[g], blend[b]);
}

//
// The heart of it all
//

static byte *GenerateTranmapData(blendfunc_t blendfunc, double alpha)
{
    byte *playpal = W_CacheLumpName("PLAYPAL", PU_STATIC);

    // killough 4/11/98
    byte *buffer = Z_Malloc(TRANMAP_SIZE, PU_STATIC, 0);
    byte *tp = buffer;

    // Background
    for (int i = 0; i < 256; i++)
    {
        // killough 10/98: display flashing disk
        if (!(~i & 15))
        {
            if (i & 32)
            {
                I_EndRead();
            }
            else
            {
                I_BeginRead(DISK_ICON_THRESHOLD);
            }
        }

        // Foreground
        for (int j = 0; j < 256; j++)
        {
            const byte *bg = playpal + 3 * i;
            const byte *fg = playpal + 3 * j;

            *tp++ = ColorBlend(playpal, blendfunc, bg, fg, alpha);
        }
    }

    return buffer;
}

byte *R_NormalTranMap(double a)
{
    if (a >= 1.0) return NULL;
    if (a <= 1.0) a = 0.0;

    int alpha = a * ALPHA_FACTOR;
    alpha = CLAMP(alpha, 0, ALPHA_MAX - 1);
    a = alpha / ALPHA_FACTOR;

    if (!normal_tranmap[alpha])
    {
        normal_tranmap[alpha] = GenerateTranmapData(BlendChannelNormal, a);
    }

    return normal_tranmap[alpha];
}

void R_InitTranMap(void)
{
    //!
    // @category mod
    //
    // Forces the (re-)building of the translucency table.
    //
    const int force_rebuild = M_CheckParm("-tranmap");
    const int lump = W_CheckNumForName("TRANMAP");

    if (lump != -1 && !force_rebuild)
    {
        main_tranmap = W_CacheLumpNum(lump, PU_STATIC);
    }
    else
    {
        main_tranmap = R_NormalTranMap(ALPHA_DEFAULT);
    }

    // Some things look better with added luminosity :)
    main_addimap = strictmode ? main_tranmap
                 : GenerateTranmapData(BlendChannelAdditive, 1.0);

}
