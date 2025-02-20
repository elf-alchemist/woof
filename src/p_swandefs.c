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

#include "i_printf.h"
#include "m_array.h"
#include "m_json.h"
#include "m_misc.h"
#include "sounds.h"

#include "p_swandefs.h"

static swan_switch_t default_switches[] = {
    { "SW1BRCOM", "SW2BRCOM", sfx_swtchn, sfx_swtchx },
    { "SW1BRN1",  "SW2BRN1",  sfx_swtchn, sfx_swtchx },
    { "SW1BRN2",  "SW2BRN2",  sfx_swtchn, sfx_swtchx },
    { "SW1BRNGN", "SW2BRNGN", sfx_swtchn, sfx_swtchx },
    { "SW1BROWN", "SW2BROWN", sfx_swtchn, sfx_swtchx },
    { "SW1COMM",  "SW2COMM",  sfx_swtchn, sfx_swtchx },
    { "SW1COMP",  "SW2COMP",  sfx_swtchn, sfx_swtchx },
    { "SW1DIRT",  "SW2DIRT",  sfx_swtchn, sfx_swtchx },
    { "SW1EXIT",  "SW2EXIT",  sfx_swtchn, sfx_swtchx },
    { "SW1GRAY",  "SW2GRAY",  sfx_swtchn, sfx_swtchx },
    { "SW1GRAY1", "SW2GRAY1", sfx_swtchn, sfx_swtchx },
    { "SW1METAL", "SW2METAL", sfx_swtchn, sfx_swtchx },
    { "SW1PIPE",  "SW2PIPE",  sfx_swtchn, sfx_swtchx },
    { "SW1SLAD",  "SW2SLAD",  sfx_swtchn, sfx_swtchx },
    { "SW1STARG", "SW2STARG", sfx_swtchn, sfx_swtchx },
    { "SW1STON1", "SW2STON1", sfx_swtchn, sfx_swtchx },
    { "SW1STON2", "SW2STON2", sfx_swtchn, sfx_swtchx },
    { "SW1STONE", "SW2STONE", sfx_swtchn, sfx_swtchx },
    { "SW1STRTN", "SW2STRTN", sfx_swtchn, sfx_swtchx },
    { "SW1BLUE",  "SW2BLUE",  sfx_swtchn, sfx_swtchx },
    { "SW1CMT",   "SW2CMT",   sfx_swtchn, sfx_swtchx },
    { "SW1GARG",  "SW2GARG",  sfx_swtchn, sfx_swtchx },
    { "SW1GSTON", "SW2GSTON", sfx_swtchn, sfx_swtchx },
    { "SW1HOT",   "SW2HOT",   sfx_swtchn, sfx_swtchx },
    { "SW1LION",  "SW2LION",  sfx_swtchn, sfx_swtchx },
    { "SW1SATYR", "SW2SATYR", sfx_swtchn, sfx_swtchx },
    { "SW1SKIN",  "SW2SKIN",  sfx_swtchn, sfx_swtchx },
    { "SW1VINE",  "SW2VINE",  sfx_swtchn, sfx_swtchx },
    { "SW1WOOD",  "SW2WOOD",  sfx_swtchn, sfx_swtchx },
    { "SW1PANEL", "SW2PANEL", sfx_swtchn, sfx_swtchx },
    { "SW1ROCK",  "SW2ROCK",  sfx_swtchn, sfx_swtchx },
    { "SW1MET2",  "SW2MET2",  sfx_swtchn, sfx_swtchx },
    { "SW1WDMET", "SW2WDMET", sfx_swtchn, sfx_swtchx },
    { "SW1BRIK",  "SW2BRIK",  sfx_swtchn, sfx_swtchx },
    { "SW1MOD1",  "SW2MOD1",  sfx_swtchn, sfx_swtchx },
    { "SW1ZIM",   "SW2ZIM",   sfx_swtchn, sfx_swtchx },
    { "SW1STON6", "SW2STON6", sfx_swtchn, sfx_swtchx },
    { "SW1TEK",   "SW2TEK",   sfx_swtchn, sfx_swtchx },
    { "SW1MARB",  "SW2MARB",  sfx_swtchn, sfx_swtchx },
    { "SW1SKULL", "SW2SKULL", sfx_swtchn, sfx_swtchx },
};

static swan_texture_t default_textures[] = {
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

static swan_flat_t default_flats[] = {
    { "NUKAGE1", "NUKAGE3", TERRAIN_SLIME, FLAT_EFFECT_NONE, 8 },
    { "FWATER1", "FWATER4", TERRAIN_WATER, FLAT_EFFECT_NONE, 8 },
    { "SWATER1", "SWATER4", TERRAIN_WATER, FLAT_EFFECT_NONE, 8 },
    { "LAVA1",   "LAVA4",   TERRAIN_LAVA,  FLAT_EFFECT_NONE, 8 },
    { "BLOOD1",  "BLOOD3",  TERRAIN_WATER, FLAT_EFFECT_NONE, 8 },
    { "RROCK05", "RROCK08", TERRAIN_NONE,  FLAT_EFFECT_NONE, 8 },
    { "SLIME01", "SLIME04", TERRAIN_NONE,  FLAT_EFFECT_NONE, 8 },
    { "SLIME05", "SLIME08", TERRAIN_NONE,  FLAT_EFFECT_NONE, 8 },
    { "SLIME09", "SLIME12", TERRAIN_NONE,  FLAT_EFFECT_NONE, 8 },
};

swan_switch_t  *swandefs_switches = NULL;
swan_texture_t *swandefs_textures = NULL;
swan_flat_t    *swandefs_flats    = NULL;
swan_terrain_t *swandefs_terrain  = NULL;

static void ParseSwanDefsSwitches(json_t *switches)
{
    const char *texture_inactive   = JS_GetStringValue(switches, "texture_inactive");
    const char *texture_active     = JS_GetStringValue(switches, "texture_active");
    int         sound_activation   = JS_GetIntegerValue(switches, "sound_activation");
    int         sound_deactivation = JS_GetIntegerValue(switches, "sound_deactivation");

    swan_switch_t swan_switch = {0};

    M_CopyLumpName(swan_switch.texture_inactive, texture_inactive);
    M_CopyLumpName(swan_switch.texture_active,   texture_active);
    swan_switch.sound_activation   = sound_activation;
    swan_switch.sound_deactivation = sound_deactivation;

    array_push(swandefs_switches, swan_switch);
}

static void ParseSwanDefsTextures(json_t *texture)
{
    const char *initial  = JS_GetStringValue(texture, "initial");
    const char *final    = JS_GetStringValue(texture, "final");
    int         duration = JS_GetIntegerValue(texture, "duration");

    swan_texture_t swan_texture = {0};

    M_CopyLumpName(swan_texture.initial, initial);
    M_CopyLumpName(swan_texture.final,   final);
    swan_texture.duration = duration;

    array_push(swandefs_textures, swan_texture);
}

static void ParseSwanDefsFlats(json_t *flat)
{
    const char *initial  = JS_GetStringValue(flat, "initial");
    const char *final    = JS_GetStringValue(flat, "final");
    int         terrain  = JS_GetIntegerValue(flat, "terrain");
    int         effect   = JS_GetIntegerValue(flat, "effect");
    int         duration = JS_GetIntegerValue(flat, "duration");

    swan_flat_t swan_flat = {0};

    M_CopyLumpName(swan_flat.initial, initial);
    M_CopyLumpName(swan_flat.final,   final);
    swan_flat.terrain  = terrain;
    swan_flat.effect   = effect;
    swan_flat.duration = duration;

    array_push(swandefs_flats, swan_flat);
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

    // Does is it even have the definitions we are looking for?
    json_t *swan_entry_switch;
    json_t *swan_switches = JS_GetObject(data, "switches");
    if (!JS_IsNull(swan_switches) || JS_IsArray(swan_switches))
    {
        JS_ArrayForEach(swan_entry_switch, swan_switches)
        {
            ParseSwanDefsSwitches(swan_switches);
        }
    }

    json_t *swan_entry_texture;
    json_t *swan_textures = JS_GetObject(data, "textures");
    if (!JS_IsNull(swan_textures) || JS_IsArray(swan_textures))
    {
        JS_ArrayForEach(swan_entry_texture, swan_textures)
        {
            ParseSwanDefsTextures(swan_textures);
        }
    }

    json_t *swan_entry_flat;
    json_t *swan_flats = JS_GetObject(data, "flats");
    if (!JS_IsNull(swan_flats) || JS_IsArray(swan_flats))
    {
        JS_ArrayForEach(swan_entry_flat, swan_flats)
        {
            ParseSwanDefsFlats(swan_flats);
        }
    }

    JS_Close("SWANDEFS");
}
