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

#include "r_defs.h"

extern void EV_ToggleLineHorizon(line_t *line);
extern void EV_SetSectorColormap(line_t *line, int32_t side);

typedef enum mbf2y_specials_e
{
  Static_LineHorizon = 1090,
  W1_LineHorizon,
  WR_LineHorizon,
  S1_LineHorizon,
  SR_LineHorizon,
  G1_LineHorizon,
  GR_LineHorizon,

  Static_SectorColormap, // 1097
  W1_SectorColormap,
  WR_SectorColormap,
  S1_SectorColormap,
  SR_SectorColormap,
  G1_SectorColormap,
  GR_SectorColormap,

  Static_HeightSecTint, // 1104
  W1_HeightSecTint,
  WR_HeightSecTint,
  S1_HeightSecTint,
  SR_HeightSecTint,
  G1_HeightSecTint,
  GR_HeightSecTint,
} mbf2y_specials_t;
