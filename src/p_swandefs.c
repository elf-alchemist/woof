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

#include "p_swandefs.h"

static swan_switch_t default_switches[] = {
    { "SW1BRCOM", "SW2BRCOM", 23, 24 },
    { "SW1BRN1",  "SW2BRN1",  23, 24 },
    { "SW1BRN2",  "SW2BRN2",  23, 24 },
    { "SW1BRNGN", "SW2BRNGN", 23, 24 },
    { "SW1BROWN", "SW2BROWN", 23, 24 },
    { "SW1COMM",  "SW2COMM",  23, 24 },
    { "SW1COMP",  "SW2COMP",  23, 24 },
    { "SW1DIRT",  "SW2DIRT",  23, 24 },
    { "SW1EXIT",  "SW2EXIT",  23, 24 },
    { "SW1GRAY",  "SW2GRAY",  23, 24 },
    { "SW1GRAY1", "SW2GRAY1", 23, 24 },
    { "SW1METAL", "SW2METAL", 23, 24 },
    { "SW1PIPE",  "SW2PIPE",  23, 24 },
    { "SW1SLAD",  "SW2SLAD",  23, 24 },
    { "SW1STARG", "SW2STARG", 23, 24 },
    { "SW1STON1", "SW2STON1", 23, 24 },
    { "SW1STON2", "SW2STON2", 23, 24 },
    { "SW1STONE", "SW2STONE", 23, 24 },
    { "SW1STRTN", "SW2STRTN", 23, 24 },
    { "SW1BLUE",  "SW2BLUE",  23, 24 },
    { "SW1CMT",   "SW2CMT",   23, 24 },
    { "SW1GARG",  "SW2GARG",  23, 24 },
    { "SW1GSTON", "SW2GSTON", 23, 24 },
    { "SW1HOT",   "SW2HOT",   23, 24 },
    { "SW1LION",  "SW2LION",  23, 24 },
    { "SW1SATYR", "SW2SATYR", 23, 24 },
    { "SW1SKIN",  "SW2SKIN",  23, 24 },
    { "SW1VINE",  "SW2VINE",  23, 24 },
    { "SW1WOOD",  "SW2WOOD",  23, 24 },
    { "SW1PANEL", "SW2PANEL", 23, 24 },
    { "SW1ROCK",  "SW2ROCK",  23, 24 },
    { "SW1MET2",  "SW2MET2",  23, 24 },
    { "SW1WDMET", "SW2WDMET", 23, 24 },
    { "SW1BRIK",  "SW2BRIK",  23, 24 },
    { "SW1MOD1",  "SW2MOD1",  23, 24 },
    { "SW1ZIM",   "SW2ZIM",   23, 24 },
    { "SW1STON6", "SW2STON6", 23, 24 },
    { "SW1TEK",   "SW2TEK",   23, 24 },
    { "SW1MARB",  "SW2MARB",  23, 24 },
    { "SW1SKULL", "SW2SKULL", 23, 24 },
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

swan_switch_t *swandefs_switches = NULL;
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

swan_texture_t *swandefs_textures = NULL;
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

swan_flat_t *swandefs_flats    = NULL;
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

static void ParseSwanDefs(void)
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
