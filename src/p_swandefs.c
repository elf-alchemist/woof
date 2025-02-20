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

swan_switch_t *swandefs_switches = NULL;
static void ParseSwanDefsSwitches(json_t *switches)
{
    const char *state_inactive     = JS_GetStringValue(switches, "state_inactive");
    const char *state_active       = JS_GetStringValue(switches, "state_active");
    int         sound_activation   = JS_GetIntegerValue(switches, "sound_activation");
    int         sound_deactivation = JS_GetIntegerValue(switches, "sound_deactivation");

    swan_switch_t swan_switch = {0};

    M_CopyLumpName(swan_switch.state_inactive, state_inactive);
    M_CopyLumpName(swan_switch.state_active,   state_active);
    swan_switch.sound_activation = sound_activation;
    swan_switch.sound_deactivation = sound_deactivation;

    array_push(swandefs_switches, swan_switch);
}

swan_texture_t *swandefs_textures = NULL;
static void ParseSwanDefsTextures(json_t *switches)
{
    const char *texture_initial = JS_GetStringValue(switches, "texture_initial");
    const char *texture_final   = JS_GetStringValue(switches, "texture_final");
    int         effect          = JS_GetIntegerValue(switches, "effect");
    int         duration        = JS_GetIntegerValue(switches, "duration");

    swan_texture_t swan_texture = {0};

    M_CopyLumpName(swan_texture.texture_initial, texture_initial);
    M_CopyLumpName(swan_texture.texture_final,   texture_final);
    swan_texture.effect = effect;
    swan_texture.duration = duration;

    array_push(swandefs_textures, swan_texture);
}

swan_flat_t *swandefs_flats    = NULL;
static void ParseSwanDefsFlats(json_t *switches)
{
    const char *flat_initial     = JS_GetStringValue(switches, "flat_initial");
    const char *flat_final       = JS_GetStringValue(switches, "flat_final");
    int         effect           = JS_GetIntegerValue(switches, "effect");
    int         duration         = JS_GetIntegerValue(switches, "duration");

    swan_flat_t swan_flat = {0};

    M_CopyLumpName(swan_flat.flat_initial, flat_initial);
    M_CopyLumpName(swan_flat.flat_final,   flat_final);
    swan_flat.effect = effect;
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
