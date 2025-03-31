//
//  Copyright (C) 1999 by
//  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
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
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

// We are referring to sprite numbers.
#include "info.h"
#include "d_items.h"
#include "doomstat.h"

//
// PSPRITE ACTIONS for waepons.
// This struct controls the weapon animations.
//
// Each entry is:
//  ammo/amunition type
//  upstate
//  downstate
//  readystate
//  atkstate, i.e. attack/fire/hit frame
//  flashstate, muzzle flash
//
weaponinfo_t    weaponinfo[NUMWEAPONS+2] =
{
  {
    // fist
    am_noammo, // ammo
    S_PUNCHUP, // upstate
    S_PUNCHDOWN, // downstate
    S_PUNCH, // readystate
    S_PUNCH1, // atkstate
    S_NULL, // flashstate

    WPF_FLEEMELEE | WPF_AUTOSWITCHFROM | WPF_NOAUTOSWITCHTO,  // flags
    1, // ammopershot
    0, // intflags

    1, // slot
    0, // slot_priority
    0, // switch_priority
    true, // initial_owned
    false, // initial_raised
    "SMFIST" // carousel_icon
  },
  {
    // pistol
    am_clip, // ammo
    S_PISTOLUP, // upstate
    S_PISTOLDOWN, // downstate
    S_PISTOL, // readystate
    S_PISTOL1, // atkstate
    S_PISTOLFLASH, // flashstate

    WPF_AUTOSWITCHFROM, // flags
    1, // ammopershot
    0, // intflags

    2, // slot
    0, // slot_priority
    6, // switch_priority
    true, // initial_owned
    true, // initial_raised
    "SMPISG" // carousel_icon
  },
  {
    // shotgun
    am_shell, // ammo
    S_SGUNUP, // upstate
    S_SGUNDOWN, // downstate
    S_SGUN, // readystate
    S_SGUN1, // atkstate
    S_SGUNFLASH1, // flashstate

    WPF_NOFLAG, // flags
    1, // ammopershot
    0, // intflags

    3, // slot
    0, // slot_priority
    7, // switch_priority
    false, // initial_owned
    false, // initial_raised
    "SMSHOT" // carousel_icon
  },
  {
    // chaingun
    am_clip, // ammo
    S_CHAINUP, // upstate
    S_CHAINDOWN, // downstate
    S_CHAIN, // readystate
    S_CHAIN1, // atkstate
    S_CHAINFLASH1, // flashstate

    WPF_NOFLAG, // flags
    1, // ammopershot
    0, // intflags

    4, // slot
    0, // slot_priority
    8, // switch_priority
    false, // initial_owned
    false, // initial_raised
    "SMMGUN" // carousel_icon
  },
  {
    // missile launcher
    am_misl, // ammo
    S_MISSILEUP, // upstate
    S_MISSILEDOWN, // downstate
    S_MISSILE, // readystate
    S_MISSILE1, // atkstate
    S_MISSILEFLASH1, // flashstate

    WPF_NOAUTOFIRE, // flags
    1, // ammopershot
    0, // intflags

    5, // slot
    0, // slot_priority
    4, // switch_priority
    false, // initial_owned
    false, // initial_raised
    "SMLAUN" // carousel_icon
  },
  {
    // plasma rifle
    am_cell, // ammo
    S_PLASMAUP, // upstate
    S_PLASMADOWN, // downstate
    S_PLASMA, // readystate
    S_PLASMA1, // atkstate
    S_PLASMAFLASH1, // flashstate

    WPF_NOFLAG, // flags
    1, // ammopershot
    0, // intflags

    6, // slot
    0, // slot_priority
    10, // switch_priority
    false, // initial_owned
    false, // initial_raised
    "SMPLAS" // carousel_icon
  },
  {
    // bfg 9000
    am_cell, // ammo
    S_BFGUP, // upstate
    S_BFGDOWN, // downstate
    S_BFG, // readystate
    S_BFG1, // atkstate
    S_BFGFLASH1, // flashstate

    WPF_NOAUTOFIRE, // flags
    40, // ammopershot
    0, // intflags

    7, // slot
    0, // slot_priority
    2, // switch_priority
    false, // initial_owned
    false, // initial_raised
    "SMBFGG" // carousel_icon
  },
  {
    // chainsaw
    am_noammo, // ammo
    S_SAWUP, // upstate
    S_SAWDOWN, // downstate
    S_SAW, // readystate
    S_SAW1, // atkstate
    S_NULL, // flashstate

    WPF_NOTHRUST | WPF_FLEEMELEE | WPF_NOAUTOSWITCHTO, // flags
    1, // ammopershot
    0, // intflags

    1, // slot
    1, // slot_priority
    5, // switch_priority
    false, // initial_owned
    false, // initial_raised
    "SMVSAW" // carousel_icon
  },
  {
    // super shotgun
    am_shell, // ammo
    S_DSGUNUP, // upstate
    S_DSGUNDOWN, // downstate
    S_DSGUN, // readystate
    S_DSGUN1, // atkstate
    S_DSGUNFLASH1, // flashstate

    WPF_NOFLAG, // flags
    2, // ammopershot
    0, // intflags

    3, // slot
    0, // slot_priority
    9, // switch_priority
    false, // initial_owned
    false, // initial_raised
    "SMSGN2" // carousel_icon
  },
  {
    0
  },
  {
    0
  },
};

//----------------------------------------------------------------------------
//
// $Log: d_items.c,v $
// Revision 1.4  1998/05/04  21:34:09  thldrmn
// commenting and reformatting
//
// Revision 1.2  1998/01/26  19:23:03  phares
// First rev with no ^Ms
//
// Revision 1.1.1.1  1998/01/19  14:03:07  rand
// Lee's Jan 19 sources
//
//
//----------------------------------------------------------------------------
