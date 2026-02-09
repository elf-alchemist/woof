//
// Copyright(C) 2026 Roman Fomin
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include <string.h>

#include "doomstat.h"
#include "dsdh_main.h"
#include "info.h"
#include "m_argv.h"
#include "m_array.h"
#include "m_hashmap.h"
#include "p_map.h"
#include "p_mobj.h"
#include "sounds.h"

mobjinfo_t *mobjinfo = NULL;
int num_mobj_types;

static int max_thing_number;

static hashmap_t *translate;

int DSDH_ThingTranslate(int thing_number);

void DSDH_MobjInfoInit(void)
{
    num_mobj_types = NUMMOBJTYPES;
    max_thing_number = NUMMOBJTYPES - 1;

    array_resize(mobjinfo, NUMMOBJTYPES);
    memcpy(mobjinfo, original_mobjinfo, NUMMOBJTYPES * sizeof(*mobjinfo));

    // don't want to reorganize info.c structure for a few tweaks...
    for (int i = 0; i < num_mobj_types; ++i)
    {
        // DEHEXTRA
        mobjinfo[i].droppeditem = MT_NULL;
        // MBF21
        mobjinfo[i].flags2           = MF2_NONE;
        mobjinfo[i].infighting_group = IG_DEFAULT;
        mobjinfo[i].projectile_group = PG_DEFAULT;
        mobjinfo[i].splash_group     = SG_DEFAULT;
        mobjinfo[i].ripsound         = sfx_None;
        mobjinfo[i].altspeed         = NO_ALTSPEED;
        mobjinfo[i].meleerange       = MELEERANGE;
        // ID24
        mobjinfo[i].flags3             = MF3_NONE;
        mobjinfo[i].respawn_min_tics   = RESPAWN_TICS;
        mobjinfo[i].respawn_dice       = RESPAWN_DICE;
        mobjinfo[i].pickup_ammo_type   = NO_INDEX;
        mobjinfo[i].pickup_weapon_type = NO_INDEX;
        mobjinfo[i].pickup_item_type   = NO_INDEX;
        mobjinfo[i].pickup_sound       = sfx_None;
        mobjinfo[i].pickup_bonus       = PICKUP_BONUS;
        mobjinfo[i].pickup_mnemonic    = NULL;
        // Eternity
        mobjinfo[i].bloodcolor = 0;
    }

    // DEHEXTRA
    mobjinfo[MT_WOLFSS].droppeditem = MT_CLIP;
    mobjinfo[MT_POSSESSED].droppeditem = MT_CLIP;
    mobjinfo[MT_SHOTGUY].droppeditem = MT_SHOTGUN;
    mobjinfo[MT_CHAINGUY].droppeditem = MT_CHAINGUN;

    // MBF21
    mobjinfo[MT_VILE].flags2 =
        MF2_SHORTMRANGE | MF2_DMGIGNORED | MF2_NOTHRESHOLD;
    mobjinfo[MT_UNDEAD].flags2 = MF2_LONGMELEE | MF2_RANGEHALF;
    mobjinfo[MT_FATSO].flags2 = MF2_MAP07BOSS1;
    mobjinfo[MT_BRUISER].flags2 = MF2_E1M8BOSS;
    mobjinfo[MT_SKULL].flags2 = MF2_RANGEHALF;
    mobjinfo[MT_SPIDER].flags2 = MF2_NORADIUSDMG | MF2_RANGEHALF
                                 | MF2_FULLVOLSOUNDS | MF2_E3M8BOSS
                                 | MF2_E4M8BOSS;
    mobjinfo[MT_BABY].flags2 = MF2_MAP07BOSS2;
    mobjinfo[MT_CYBORG].flags2 = MF2_NORADIUSDMG | MF2_HIGHERMPROB
                                 | MF2_RANGEHALF | MF2_FULLVOLSOUNDS
                                 | MF2_E2M8BOSS | MF2_E4M6BOSS;

    mobjinfo[MT_BRUISER].projectile_group = PG_BARON;
    mobjinfo[MT_KNIGHT].projectile_group = PG_BARON;

    mobjinfo[MT_BRUISERSHOT].altspeed = IntToFixed(20);
    mobjinfo[MT_TROOPSHOT].altspeed = IntToFixed(20);
    mobjinfo[MT_HEADSHOT].altspeed = IntToFixed(20);

    for (int i = S_SARG_RUN1; i <= S_SARG_PAIN2; ++i)
    {
        states[i].flags |= STATEF_SKILL5FAST;
    }

    // ID24
    mobjinfo[MT_MISC4].flags3 |= MF3_SPECIALSTAYSCOOP;
    mobjinfo[MT_MISC5].flags3 |= MF3_SPECIALSTAYSCOOP;
    mobjinfo[MT_MISC6].flags3 |= MF3_SPECIALSTAYSCOOP;
    mobjinfo[MT_MISC7].flags3 |= MF3_SPECIALSTAYSCOOP;
    mobjinfo[MT_MISC8].flags3 |= MF3_SPECIALSTAYSCOOP;
    mobjinfo[MT_MISC9].flags3 |= MF3_SPECIALSTAYSCOOP;

    // Woof! randomly mirrored death animations
    for (int i = MT_PLAYER; i <= MT_KEEN; ++i)
    {
        switch (i)
        {
            case MT_FIRE:
            case MT_TRACER:
            case MT_SMOKE:
            case MT_FATSHOT:
            case MT_BRUISERSHOT:
            case MT_CYBORG:
                continue;
        }
        mobjinfo[i].flags_extra |= MFX_MIRROREDCORPSE;
    }

    mobjinfo[MT_PUFF].flags_extra |= MFX_MIRROREDCORPSE;
    mobjinfo[MT_BLOOD].flags_extra |= MFX_MIRROREDCORPSE;

    for (int i = MT_MISC61; i <= MT_MISC69; ++i)
    {
        mobjinfo[i].flags_extra |= MFX_MIRROREDCORPSE;
    }

    mobjinfo[MT_DOGS].flags_extra |= MFX_MIRROREDCORPSE;

    //!
    // @category game
    //
    // Press beta emulation mode (complevel mbf only).
    //
    beta_emulation = M_CheckParm("-beta");
    if (beta_emulation)
    {
        // killough 10/98: beta lost soul has different behavior frames
        mobjinfo[MT_SKULL].spawnstate = S_BSKUL_STND;
        mobjinfo[MT_SKULL].seestate = S_BSKUL_RUN1;
        mobjinfo[MT_SKULL].painstate = S_BSKUL_PAIN1;
        mobjinfo[MT_SKULL].missilestate = S_BSKUL_ATK1;
        mobjinfo[MT_SKULL].deathstate = S_BSKUL_DIE1;
        mobjinfo[MT_SKULL].damage = 1;
    }
    // This code causes MT_SCEPTRE and MT_BIBLE to not spawn on the map,
    // which causes desync in Eviternity.wad demos.
#ifdef MBF_STRICT
    else
    {
        mobjinfo[MT_SCEPTRE].doomednum = mobjinfo[MT_BIBLE].doomednum = -1;
    }
#endif

    if (demo_version < DV_ID24)
    {
        return;
    }

    // array_resize(mobjinfo, NUMMOBJTYPES_ID24);
    // memcpy(mobjinfo + num_mobj_types, id24_mobjinfo, NUMMOBJTYPES_ID24 *
    // sizeof(*mobjinfo));

    // TODO: wtf?
    for (int i = MT_GHOUL; i < MT_INCINERATOR; i++)
    {
        DSDH_ThingTranslate(i);
    }

    for (int i = 0; i < NUMMOBJTYPES_ID24; i++)
    {
        int32_t real_index = DSDH_ThingTranslate(ID24_NEG_OFFSET + i);
        mobjinfo[real_index].doomednum = id24_mobjinfo[i].doomednum;
        mobjinfo[real_index].spawnstate = DSDH_StateTranslate(id24_mobjinfo[i].spawnstate);
        mobjinfo[real_index].spawnhealth = id24_mobjinfo[i].spawnhealth;
        mobjinfo[real_index].seestate = DSDH_StateTranslate(id24_mobjinfo[i].seestate);
        mobjinfo[real_index].seesound = DSDH_SoundTranslate(id24_mobjinfo[i].seesound);
        mobjinfo[real_index].reactiontime = id24_mobjinfo[i].reactiontime;
        mobjinfo[real_index].attacksound = DSDH_SoundTranslate(id24_mobjinfo[i].attacksound);
        mobjinfo[real_index].painstate = DSDH_StateTranslate(id24_mobjinfo[i].painstate);
        mobjinfo[real_index].painchance = id24_mobjinfo[i].painchance;
        mobjinfo[real_index].painsound = DSDH_SoundTranslate(id24_mobjinfo[i].painsound);
        mobjinfo[real_index].meleestate = DSDH_StateTranslate(id24_mobjinfo[i].meleestate);
        mobjinfo[real_index].missilestate = DSDH_StateTranslate(id24_mobjinfo[i].missilestate);
        mobjinfo[real_index].deathstate = DSDH_StateTranslate(id24_mobjinfo[i].deathstate);
        mobjinfo[real_index].xdeathstate = DSDH_StateTranslate(id24_mobjinfo[i].xdeathstate);
        mobjinfo[real_index].deathsound = DSDH_SoundTranslate(id24_mobjinfo[i].deathsound);
        mobjinfo[real_index].speed = id24_mobjinfo[i].speed;
        mobjinfo[real_index].radius = id24_mobjinfo[i].radius;
        mobjinfo[real_index].height = id24_mobjinfo[i].height;
        mobjinfo[real_index].mass = id24_mobjinfo[i].mass;
        mobjinfo[real_index].damage = id24_mobjinfo[i].damage;
        mobjinfo[real_index].activesound = DSDH_SoundTranslate(id24_mobjinfo[i].activesound);
        mobjinfo[real_index].flags = id24_mobjinfo[i].flags;
        mobjinfo[real_index].raisestate = DSDH_StateTranslate(id24_mobjinfo[i].raisestate);
        mobjinfo[real_index].droppeditem = id24_mobjinfo[i].droppeditem;
        mobjinfo[real_index].flags2 = id24_mobjinfo[i].flags2;
        mobjinfo[real_index].infighting_group = id24_mobjinfo[i].infighting_group;
        mobjinfo[real_index].projectile_group = id24_mobjinfo[i].projectile_group;
        mobjinfo[real_index].splash_group = id24_mobjinfo[i].splash_group;
        mobjinfo[real_index].ripsound = DSDH_SoundTranslate(id24_mobjinfo[i].ripsound);
        mobjinfo[real_index].altspeed = id24_mobjinfo[i].altspeed;
        mobjinfo[real_index].meleerange = id24_mobjinfo[i].meleerange;
        mobjinfo[real_index].flags3 = id24_mobjinfo[i].flags3;
        mobjinfo[real_index].respawn_min_tics = id24_mobjinfo[i].respawn_min_tics;
        mobjinfo[real_index].respawn_dice = id24_mobjinfo[i].respawn_dice;
        mobjinfo[real_index].pickup_ammo_type = id24_mobjinfo[i].pickup_ammo_type;
        mobjinfo[real_index].pickup_ammo_category = id24_mobjinfo[i].pickup_ammo_category;
        mobjinfo[real_index].pickup_weapon_type = id24_mobjinfo[i].pickup_weapon_type;
        mobjinfo[real_index].pickup_item_type = id24_mobjinfo[i].pickup_item_type;
        mobjinfo[real_index].pickup_bonus = id24_mobjinfo[i].pickup_bonus;
        mobjinfo[real_index].pickup_sound = DSDH_SoundTranslate(id24_mobjinfo[i].pickup_sound);
        mobjinfo[real_index].pickup_mnemonic = id24_mobjinfo[i].pickup_mnemonic;
        mobjinfo[real_index].xlat_lump = id24_mobjinfo[i].xlat_lump;
    }
}

int DSDH_ThingTranslate(int thing_number)
{
    max_thing_number = MAX(max_thing_number, thing_number);

    if (thing_number < NUMMOBJTYPES)
    {
        return thing_number;
    }

    if (!translate)
    {
        translate = hashmap_init(256);
    }

    int index;
    if (hashmap_get(translate, thing_number, &index))
    {
        return index;
    }

    index = num_mobj_types;
    hashmap_put(translate, thing_number, &index);

    mobjinfo_t mobj = {
        // DEHEXTRA
        .droppeditem = MT_NULL,
        // MBF21
        .infighting_group = IG_DEFAULT,
        .projectile_group = PG_DEFAULT,
        .splash_group = SG_DEFAULT,
        .altspeed = NO_ALTSPEED,
        .meleerange = MELEERANGE,
        // ID24
        .respawn_min_tics = RESPAWN_TICS,
        .respawn_dice = RESPAWN_DICE,
        .pickup_ammo_type = NO_INDEX,
        .pickup_weapon_type = NO_INDEX,
        .pickup_item_type = NO_INDEX,
        .pickup_bonus = PICKUP_BONUS,
    };
    array_push(mobjinfo, mobj);
    ++num_mobj_types;

    return index;
}

int DSDH_MobjInfoGetNewIndex(void)
{
    ++max_thing_number;
    return DSDH_ThingTranslate(max_thing_number);
}
