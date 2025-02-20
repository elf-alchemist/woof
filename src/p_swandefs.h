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

// Improved switch texture definition
typedef struct {
    char state_inactive[9];
    char state_active[9];
    int  sound_activation;
    int  sound_deactivation;
} swan_switch_t;

// Improved animated texture definition
typedef struct {
    char texture_initial[9];
    char texture_final[9];
    int  effect;
    int  duration;
} swan_texture_t;

// Improved animated flat definition
typedef struct {
    char flat_initial[9];
    char flat_final[9];
    int  effect;
    int  duration;
} swan_flat_t;

enum SwanFlatEffect {
    SWAN_FLAT_NONE,
    SWAN_FLAT_SMMU,
    SWAN_FLAT_MAX,
};
