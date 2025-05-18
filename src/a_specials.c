//
//  Copyright (C) 2025 Guilherme Miranda
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

#include "a_specials.h"

#include "doomdata.h"
#include "p_spec.h"
#include "r_state.h"

//
// If (tag == 0) toggle own horizon, good for the static line
// If (tag != 0) taggle all tagged lines' horizon
//
void EV_ToggleLineHorizon(line_t *line)
{
  if (!line->args[0])
  {
    line->flags ^= ML_HORIZON;
  }
  else
  {
    for (int l = -1; (l = P_FindLineFromLineTag(line, l)) >= 0;)
    {
      line[l].flags ^= ML_HORIZON;
    }
  }
}

//
// Boom's 242 requires HeightSec and playerview within the sector -- boo!
// ID24's tinting applies independent of playerview or HeightSec -- yay!
// MBF2y's applies independent of HeightSec, but needs playerview -- still useful!
//
void EV_SetSectorColormap(line_t *line, int32_t side)
{
  int32_t colormap_index = side ? sides[line->sidenum[0]].bottomindex
                                : sides[line->sidenum[0]].topindex;

  if (!line->args[0])
  {
    line->frontsector->colormap = colormap_index;
  }
  else
  {
    for (int s = -1; (s = P_FindSectorFromLineTag(line, s)) >= 0;)
    {
      sectors[s].colormap = colormap_index;
    }
  }
}
