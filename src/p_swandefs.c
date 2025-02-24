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

#include "doomdef.h"
#include "i_printf.h"
#include "m_array.h"
#include "m_json.h"
#include "sounds.h"
#include "w_wad.h"

#include "p_swandefs.h"

// [EA]
// Partly duplicated from header because the anim loop requires Index values,
// which needs to be calculated at initialization time, not hardcoded in the
// default values.

typedef struct {
    char inactive[9];
    char active[9];
    int  delay;
    int  sound;
    int  exit;
} default_switches_t;

typedef struct {
    char initial[9];
    char final[9];
    int  duration;
} default_texture_t;

typedef struct {
    char           initial[9];
    char           final[9];
    swan_terrain_t terrain;
    swan_effect_t  effect;
    int            duration;
} default_flat_t;

static default_switches_t default_switches[] = {
    { "SW1BRCOM", "SW2BRCOM", 35, sfx_swtchn, sfx_swtchx },
    { "SW1BRN1",  "SW2BRN1",  35, sfx_swtchn, sfx_swtchx },
    { "SW1BRN2",  "SW2BRN2",  35, sfx_swtchn, sfx_swtchx },
    { "SW1BRNGN", "SW2BRNGN", 35, sfx_swtchn, sfx_swtchx },
    { "SW1BROWN", "SW2BROWN", 35, sfx_swtchn, sfx_swtchx },
    { "SW1COMM",  "SW2COMM",  35, sfx_swtchn, sfx_swtchx },
    { "SW1COMP",  "SW2COMP",  35, sfx_swtchn, sfx_swtchx },
    { "SW1DIRT",  "SW2DIRT",  35, sfx_swtchn, sfx_swtchx },
    { "SW1EXIT",  "SW2EXIT",  35, sfx_swtchn, sfx_swtchx },
    { "SW1GRAY",  "SW2GRAY",  35, sfx_swtchn, sfx_swtchx },
    { "SW1GRAY1", "SW2GRAY1", 35, sfx_swtchn, sfx_swtchx },
    { "SW1METAL", "SW2METAL", 35, sfx_swtchn, sfx_swtchx },
    { "SW1PIPE",  "SW2PIPE",  35, sfx_swtchn, sfx_swtchx },
    { "SW1SLAD",  "SW2SLAD",  35, sfx_swtchn, sfx_swtchx },
    { "SW1STARG", "SW2STARG", 35, sfx_swtchn, sfx_swtchx },
    { "SW1STON1", "SW2STON1", 35, sfx_swtchn, sfx_swtchx },
    { "SW1STON2", "SW2STON2", 35, sfx_swtchn, sfx_swtchx },
    { "SW1STONE", "SW2STONE", 35, sfx_swtchn, sfx_swtchx },
    { "SW1STRTN", "SW2STRTN", 35, sfx_swtchn, sfx_swtchx },
    { "SW1BLUE",  "SW2BLUE",  35, sfx_swtchn, sfx_swtchx },
    { "SW1CMT",   "SW2CMT",   35, sfx_swtchn, sfx_swtchx },
    { "SW1GARG",  "SW2GARG",  35, sfx_swtchn, sfx_swtchx },
    { "SW1GSTON", "SW2GSTON", 35, sfx_swtchn, sfx_swtchx },
    { "SW1HOT",   "SW2HOT",   35, sfx_swtchn, sfx_swtchx },
    { "SW1LION",  "SW2LION",  35, sfx_swtchn, sfx_swtchx },
    { "SW1SATYR", "SW2SATYR", 35, sfx_swtchn, sfx_swtchx },
    { "SW1SKIN",  "SW2SKIN",  35, sfx_swtchn, sfx_swtchx },
    { "SW1VINE",  "SW2VINE",  35, sfx_swtchn, sfx_swtchx },
    { "SW1WOOD",  "SW2WOOD",  35, sfx_swtchn, sfx_swtchx },
    { "SW1PANEL", "SW2PANEL", 35, sfx_swtchn, sfx_swtchx },
    { "SW1ROCK",  "SW2ROCK",  35, sfx_swtchn, sfx_swtchx },
    { "SW1MET2",  "SW2MET2",  35, sfx_swtchn, sfx_swtchx },
    { "SW1WDMET", "SW2WDMET", 35, sfx_swtchn, sfx_swtchx },
    { "SW1BRIK",  "SW2BRIK",  35, sfx_swtchn, sfx_swtchx },
    { "SW1MOD1",  "SW2MOD1",  35, sfx_swtchn, sfx_swtchx },
    { "SW1ZIM",   "SW2ZIM",   35, sfx_swtchn, sfx_swtchx },
    { "SW1STON6", "SW2STON6", 35, sfx_swtchn, sfx_swtchx },
    { "SW1TEK",   "SW2TEK",   35, sfx_swtchn, sfx_swtchx },
    { "SW1MARB",  "SW2MARB",  35, sfx_swtchn, sfx_swtchx },
    { "SW1SKULL", "SW2SKULL", 35, sfx_swtchn, sfx_swtchx },
};

static default_texture_t default_textures[] = {
    { "BLODGR1",  "BLODGR4",  8 },
    { "SLADRIP1", "SLADRIP3", 8 },
    { "BLODRIP1", "BLODRIP4", 8 },
    { "FIREWALA", "FIREWALL", 8 },
    { "GSTFONT1", "GSTFONT3", 8 },
    { "FIRELAV3", "FIRELAVA", 8 },
    { "FIREMAG1", "FIREMAG3", 8 },
    { "FIREBLU1", "FIREBLU2", 8 },
    { "ROCKRED1", "ROCKRED3", 8 },
    { "BFALL1",   "BFALL4",   8 },
    { "SFALL1",   "SFALL4",   8 },
    { "WFALL1",   "WFALL4",   8 },
    { "DBRAIN1",  "DBRAIN4",  8 },
};

static default_flat_t default_flats[] = {
    { "NUKAGE1", "NUKAGE3", TERRAIN_SLIME, EFFECT_NONE, 8 },
    { "FWATER1", "FWATER4", TERRAIN_WATER, EFFECT_NONE, 8 },
    { "SWATER1", "SWATER4", TERRAIN_WATER, EFFECT_NONE, 8 },
    { "LAVA1",   "LAVA4",   TERRAIN_LAVA,  EFFECT_NONE, 8 },
    { "BLOOD1",  "BLOOD3",  TERRAIN_WATER, EFFECT_NONE, 8 },
    { "RROCK05", "RROCK08", TERRAIN_NONE,  EFFECT_NONE, 8 },
    { "SLIME01", "SLIME04", TERRAIN_NONE,  EFFECT_NONE, 8 },
    { "SLIME05", "SLIME08", TERRAIN_NONE,  EFFECT_NONE, 8 },
    { "SLIME09", "SLIME12", TERRAIN_NONE,  EFFECT_NONE, 8 },
};

swan_switch_t  *swandefs_switches = NULL;
swan_texture_t *swandefs_textures = NULL;
swan_flat_t    *swandefs_flats    = NULL;
swan_terrain_t *swandefs_terrain  = NULL;
swan_effect_t  *swandefs_effect   = NULL;

int swan_count_switch  = 0;
int swan_count_texture = 0;
int swan_count_flat    = 0;

void P_SwanSwitch(const char *inactive, const char *active, int delay,
                  int sound, int exit)
{
    swan_switch_t swan_switch = {0};

    swan_switch.inactive = W_CheckNumForName(inactive);
    swan_switch.active   = W_CheckNumForName(active);
    swan_switch.delay    = delay;
    swan_switch.sound    = sound;
    swan_switch.exit     = exit;

    array_push(swandefs_switches, swan_switch);
    swan_count_switch++;
}

void P_SwanTexture(const char *initial, const char *final, int duration)
{
    swan_texture_t swan_texture = {0};

    swan_texture.initial  = W_CheckNumForName(initial);
    swan_texture.final    = W_CheckNumForName(final);
    swan_texture.count    = swan_texture.final - swan_texture.initial + 1;
    swan_texture.duration = duration;

    array_push(swandefs_textures, swan_texture);
    swan_count_texture++;
}

void P_SwanFlat(const char *initial, const char *final, int terrain,
                int effect, int duration)
{
    swan_flat_t swan_flat = {0};

    swan_flat.initial  = W_CheckNumForName(initial);
    swan_flat.final    = W_CheckNumForName(final);
    swan_flat.count    = swan_flat.final - swan_flat.initial + 1;
    swan_flat.duration = duration;

    for (int flat = swan_flat.initial; flat <= swan_flat.final; flat++)
    {
        swandefs_terrain[flat] = terrain;
        swandefs_effect[flat] = effect;
    }

    array_push(swandefs_flats, swan_flat);
    swan_count_flat++;

}

void P_InitSwanDefs(void)
{
    // Does the JSON lump even exist?
    json_t *json = JS_Open("SWANDEFS", "swandefs", (version_t){1, 0, 0});
    if (json == NULL)
    {
        return;
    }

    // Does lump actually have any data?
    json_t *data = JS_GetObject(json, "data");
    if (JS_IsNull(data) || !JS_IsObject(data))
    {
        I_Printf(VB_WARNING, "SWANDEFS: data object not defined");
        JS_Close("SWANDEFS");
        return;
    }

    // Always parse vanilla definitions before
    default_switches_t *vanilla_switch;
    array_foreach(vanilla_switch, default_switches)
    {
        P_SwanSwitch(vanilla_switch->inactive,
                     vanilla_switch->active,
                     vanilla_switch->delay,
                     vanilla_switch->sound,
                     vanilla_switch->exit);
    }

    default_texture_t *vanilla_texture;
    array_foreach(vanilla_texture, default_textures)
    {
        P_SwanTexture(vanilla_texture->initial,
                      vanilla_texture->final,
                      vanilla_texture->duration);
    }

    default_flat_t *vanilla_flat;
    array_foreach(vanilla_flat, default_flats)
    {
        P_SwanFlat(vanilla_flat->initial,
                   vanilla_flat->final,
                   vanilla_flat->terrain,
                   vanilla_flat->effect,
                   vanilla_flat->duration);
    }

    // Does is it even have the definitions we are looking for?
    json_t *switch_p;
    json_t *swan_switches = JS_GetObject(data, "switches");
    if (!JS_IsNull(swan_switches) || JS_IsArray(swan_switches))
    {
        JS_ArrayForEach(switch_p, swan_switches)
        {
            const char *inactive = JS_GetStringValue(switch_p, "inactive");
            const char *active   = JS_GetStringValue(switch_p, "active");
            double      seconds  = JS_GetNumberValue(switch_p, "delay");
            int         sound    = JS_GetIntegerValue(switch_p, "sound");
            int         exit     = JS_GetIntegerValue(switch_p, "exit");

            P_SwanSwitch(inactive, active, (seconds * TICRATE), sound, exit);
        }
    }

    json_t *texture;
    json_t *swan_textures = JS_GetObject(data, "textures");
    if (!JS_IsNull(swan_textures) || JS_IsArray(swan_textures))
    {
        JS_ArrayForEach(texture, swan_textures)
        {
            const char *initial = JS_GetStringValue(texture, "initial");
            const char *final   = JS_GetStringValue(texture, "final");
            double      seconds = JS_GetNumberValue(texture, "duration");

            P_SwanTexture(initial, final, (seconds * TICRATE));
        }
    }

    json_t *flat;
    json_t *swan_flats = JS_GetObject(data, "flats");
    if (!JS_IsNull(swan_flats) || JS_IsArray(swan_flats))
    {
        JS_ArrayForEach(flat, swan_flats)
        {
            const char *initial = JS_GetStringValue(flat, "initial");
            const char *final   = JS_GetStringValue(flat, "final");
            int         terrain = JS_GetIntegerValue(flat, "terrain");
            int         effect  = JS_GetIntegerValue(flat, "effect");
            int         seconds = JS_GetNumberValue(flat, "duration");

            P_SwanFlat(initial, final, terrain, effect, (seconds * TICRATE));
        }
    }

    JS_Close("SWANDEFS");
}
