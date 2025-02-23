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
    int inactive;
    int active;
    int delay;
    int sound;
    int exit;
} swan_switch_t;

// Improved animated texture definition
typedef struct {
    int initial;
    int final;
    int count;
    int duration;
} swan_texture_t;

// Improved animated flat definition
typedef struct {
    int initial;
    int final;
    int count;
    int duration;
} swan_flat_t;

extern swan_switch_t  *swandefs_switches;
extern swan_texture_t *swandefs_textures;
extern swan_flat_t    *swandefs_flats;
extern swan_terrain_t *swandefs_terrain; // R_InitFlats defaults all to NONE
extern swan_effect_t  *swandefs_effect;  // R_InitFlats defaults all to NONE

extern int swan_count_switch;
extern int swan_count_texture;
extern int swan_count_flat;

void P_SwanSwitch(const char *inactive, const char *active, int delay,
                  int sound, int exit);
void P_SwanTexture(const char *initial, const char *final, int duration);
void P_SwanFlat(const char *initial, const char *final, int terrain,
                int effect, int duration);
void P_InitSwanDefs(void);

#endif // __P_SWANDEFS__
