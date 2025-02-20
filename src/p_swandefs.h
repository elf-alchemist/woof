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

#ifndef __P_SWANDEFS__
#define __P_SWANDEFS__

// Different SFX on different surfaces
typedef enum {
    TERRAIN_NONE,
    TERRAIN_WATER,
    TERRAIN_SLIME,
    TERRAIN_LAVA,
    TERRAIN_MAX,
} swan_terrain_t;

// Visual distortions on liquids
typedef enum {
    FLAT_EFFECT_NONE,
    FLAT_EFFECT_SMMU,
    FLAT_EFFECT_MAX,
} swan_effect_t;

// Improved switch texture definition
typedef struct {
    char texture_inactive[9];
    char texture_active[9];
    int  sound_activation;
    int  sound_deactivation;
} swan_switch_t;

// Improved animated texture definition
typedef struct {
    char initial[9];
    char final[9];
    int  duration;
} swan_texture_t;

// Improved animated flat definition
typedef struct {
    char           initial[9];
    char           final[9];
    swan_terrain_t terrain;
    swan_effect_t  effect;
    int            duration;
} swan_flat_t;


#endif // __P_SWANDEFS__
