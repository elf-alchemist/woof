//
//  Copyright (C) 1999 by
//  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
//  Copyright (C) 1993-2008 Raven Software
//  Copyright (C) 2005-2014 Simon Howard
//  Copyright (C) 2013 James Haley et al.
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
//  all external data is defined here
//  most of the data is loaded into different structures at run time
//  some internal structures shared by many modules are here
//
//-----------------------------------------------------------------------------

#ifndef __DOOMDATA__
#define __DOOMDATA__

#include "doomtype.h"
#include "m_fixed.h"

typedef enum {
  MFMT_Invalid,
  MFMT_Doom,
  MFMT_Hexen,
} mapformat_t;

extern mapformat_t mapformat;

//
// Map level types.
// The following data structures define the persistent format
// used in the lumps of the WAD files.
//

// Lump order in a map WAD: each map needs a couple of lumps
// to provide a complete scene geometry description.
enum {
  ML_LABEL,             // A separator, name, ExMx or MAPxx
  ML_THINGS,            // Monsters, items..
  ML_LINEDEFS,          // LineDefs, from editing
  ML_SIDEDEFS,          // SideDefs, from editing
  ML_VERTEXES,          // Vertices, edited and BSP splits generated
  ML_SEGS,              // LineSegs, from LineDefs split by BSP
  ML_SSECTORS,          // SubSectors, list of LineSegs
  ML_NODES,             // BSP nodes
  ML_SECTORS,           // Sectors, from editing
  ML_REJECT,            // LUT, sector-sector visibility
  ML_BLOCKMAP,          // LUT, motion clipping, walls/grid element
  ML_BEHAVIOR,          // Hexen-format, ACS byte code
};

// A single Vertex.
typedef struct {
  short x,y;
} mapvertex_t;

// A SideDef, defining the visual appearance of a wall,
// by setting textures and offsets.
typedef struct {
  short textureoffset;
  short rowoffset;
  char  toptexture[8];
  char  bottomtexture[8];
  char  midtexture[8];
  short sector;  // Front sector, towards viewer.
} mapsidedef_t;

// A LineDef, as used for editing, and as input to the BSP builder.

typedef struct {
  // [FG] extended nodes
  unsigned short v1;
  unsigned short v2;
  unsigned short flags;
  short special;
  short tag;
  unsigned short sidenum[2];  // sidenum[1] will be -1 (NO_INDEX) if one sided
} maplinedef_t;

typedef struct {
  // [FG] extended nodes
  unsigned short v1;
  unsigned short v2;
  unsigned short flags;
  byte special;
  byte arg1;
  byte arg2;
  byte arg3;
  byte arg4;
  byte arg5;
  unsigned short sidenum[2];
} maplinedef_hexen_t;

//
// LineDef attributes.
//

// Solid, is an obstacle.
#define ML_BLOCKING             1

// Blocks monsters only.
#define ML_BLOCKMONSTERS        2

// Backside will not be drawn if not two sided.
#define ML_TWOSIDED             4

// If a texture is pegged, the texture will have
// the end exposed to air held constant at the
// top or bottom of the texture (stairs or pulled
// down things) and will move with a height change
// of one of the neighbor sectors.
// Unpegged textures always have the first row of
// the texture at the top pixel of the line for both
// top and bottom textures (use next to windows).

// upper texture unpegged
#define ML_DONTPEGTOP           8

// lower texture unpegged
#define ML_DONTPEGBOTTOM        16

// In AutoMap: don't map as two sided: IT'S A SECRET!
#define ML_SECRET               32

// Sound rendering: don't let sound cross two of these.
#define ML_SOUNDBLOCK           64

// Don't draw on the automap at all.
#define ML_DONTDRAW             128

// Set if already seen, thus drawn in automap.
#define ML_MAPPED               256

//jff 3/21/98 Set if line absorbs use by player
//allow multiple push/switch triggers to be used on one push
#define ML_PASSUSE      512

// Reserved by EE
// SoM 9/02/02: 3D Middletexture flag!
#define ML_3DMIDTEX             1024

// haleyjd 05/02/06: Although it was believed until now that a reserved line
// flag was unnecessary, a problem with Ultimate DOOM E2M7 has disproven this
// theory. It has roughly 1000 linedefs with 0xFE00 masked into the flags, so
// making the next line flag reserved and using it to toggle off ALL extended
// flags will preserve compatibility for such maps. I have been told this map
// is one of the first ever created, so it may have something to do with that.
#define ML_RESERVED             2048

// mbf21
#define ML_BLOCKLANDMONSTERS    4096
#define ML_BLOCKPLAYERS         8192

// Sector definition, from editing.
typedef struct {
  short floorheight;
  short ceilingheight;
  char  floorpic[8];
  char  ceilingpic[8];
  short lightlevel;
  short special;
  short tag;
} mapsector_t;

// SubSector, as generated by BSP.
typedef struct {
  // [FG] extended nodes
  unsigned short numsegs;
  unsigned short firstseg;    // Index of first one; segs are stored sequentially.
} mapsubsector_t;

// LineSeg, generated by splitting LineDefs
// using partition lines selected by BSP builder.
typedef struct {
  // [FG] extended nodes
  unsigned short v1;
  unsigned short v2;
  short angle;
  unsigned short linedef;
  short side;
  short offset;
} mapseg_t;

// BSP node structure.

// Indicate a leaf.
#define NF_SUBSECTOR_VANILLA 0x8000
#define NF_SUBSECTOR    0x80000000
 // [FG] extended nodes
#define NO_INDEX        ((unsigned short)-1)

typedef struct {
  short x;  // Partition line from (x,y) to x+dx,y+dy)
  short y;
  short dx;
  short dy;
  // Bounding box for each child, clip against view frustum.
  short bbox[2][4];
  // If NF_SUBSECTOR its a subsector, else it's a node of another subtree.
  unsigned short children[2];
} mapnode_t;

// Thing definition, position, orientation and type,
// plus skill/visibility flags and attributes.
typedef struct {
  short x;
  short y;
  short angle;
  short type;
  short options;
} mapthing_doom_t;

typedef struct {
  short tid;
  short x;
  short y;
  short height;
  short angle;
  short type;
  short options;
  byte special;
  byte arg1;
  byte arg2;
  byte arg3;
  byte arg4;
  byte arg5;
} mapthing_hexen_t;

// haleyjd 03/03/07: New mapthing_t structure. The structures above are used to
// read things from the wad lump, but this new mapthing_t is used to store the
// data in memory now. This eliminates some weirdness and redundancies

typedef struct mapthing_s
{
  short tid;       // scripting id
  fixed_t x;         // x coord
  fixed_t y;         // y coord
  fixed_t height;    // z height relative to floor
  short angle;     // angle in wad format
  short type;      // doomednum
  int   options;   // bitflags

  byte special; // scripting special
  byte arg1;    // arguments for special
  byte arg2;
  byte arg3;
  byte arg4;
  byte arg5;
} mapthing_t;

typedef enum {
  HMTF_EASY        = 0x0001,
  HMTF_NORMAL      = 0x0002,
  HMTF_HARD        = 0x0004,
  HMTF_AMBUSH      = 0x0008,
  HMTF_DORMANT     = 0x0010,
  HMTF_FIGHTER     = 0x0020,
  HMTF_CLERIC      = 0x0040,
  HMTF_MAGE        = 0x0080,
  HMTF_GSINGLE     = 0x0100,
  HMTF_GCOOP       = 0x0200,
  HMTF_GDEATHMATCH = 0x0400,
  HMTF_FRIEND      = 0x0800,
} mapthing_hexen_flags_t;

typedef enum {
  SPAC_CROSS  = 0, // when player crosses line
  SPAC_USE    = 1, // when player uses line
  SPAC_MCROSS = 2, // when monster crosses line
  SPAC_IMPACT = 3, // when projectile hits line
  SPAC_PUSH   = 4, // when player/monster pushes line
  SPAC_PCROSS = 5, // when projectile crosses line
} spac_flags_t;

#define HMLF_REPEAT     0x0200  // special is repeatable
#define HMLF_SPAC_SHIFT 10
#define HMLF_SPAC_MASK  0x1c00
#define GET_SPAC(flags) ((flags&HMLF_SPAC_MASK)>>HMLF_SPAC_SHIFT)

//
// Every line action special
// https://doomwiki.org/wiki/Action_specials
//

typedef enum {
  Polyobj_StartLine = 1,
  Polyobj_RotateLeft,
  Polyobj_RotateRight,
  Polyobj_Move,
  Polyobj_ExplicitLine,
  Polyobj_MoveTimes8,
  Polyobj_DoorSwing,
  Polyobj_DoorSlide,

  Door_Close = 10,
  Door_Open,
  Door_Raise,
  Door_LockedRaise,

  Floor_LowerByValue = 20,
  Floor_LowerToLowest,
  Floor_LowerToNearest,
  Floor_RaiseByValue,
  Floor_RaiseToHighest,
  Floor_RaiseToNearest,
  Stairs_BuildDown,
  Stairs_BuildUp,
  Floor_RaiseAndCrush,
  Pillar_Build,
  Pillar_Open,
  Stairs_BuildDownSync,
  Stairs_BuildUpSync,

  Floor_RaiseByValueTimes8 = 35,
  Floor_LowerByValueTimes8,

  Ceiling_LowerByValue = 40,
  Ceiling_RaiseByValue,
  Ceiling_CrushAndRaise,
  Ceiling_LowerAndCrush,
  Ceiling_CrushStop,
  Ceiling_CrushRaiseAndStay,
  Floor_CrushStop,

  Plat_PerpetualRaise = 60,
  Plat_Stop,
  Plat_DownWaitUpStay,
  Plat_DownByValue,
  Plat_UpWaitDownStay,
  Plat_UpByValue,
  Floor_LowerInstant,
  Floor_RaiseInstant,
  Floor_MoveToValueTimes8,
  Ceiling_MoveToValueTimes8,
  Teleport,
  Teleport_NoFog,
  ThrustThing,
  DamageThing,
  Teleport_NewMap,
  Teleport_EndGame,

  ACS_Execute = 80,
  ACS_Suspend,
  ACS_Terminate,
  ACS_LockedExecute,

  Polyobj_OR_RotateLeft = 90,
  Polyobj_OR_RotateRight,
  Polyobj_OR_Move,
  Polyobj_OR_MoveTimes8,
  Pillar_BuildAndCrush,
  FloorAndCeiling_LowerByValue,
  FloorAndCeiling_RaiseByValue,
  Ceiling_LowerAndCrushDist,
  Sector_SetTranslucent,
  Floor_RaiseAndCrushDoom,
  Scroll_Texture_Left,
  Scroll_Texture_Model,
  Scroll_Texture_Right,
  Scroll_Texture_Up,
  Ceiling_CrushAndRaiseSilentDist,
  Door_WaitRaise,
  Door_WaitClose,
  Line_SetPortalTarget,
  Light_ForceLightning,
  Light_RaiseByValue,
  Light_RaiseByValueAlt,
  Light_LowerByValue,
  Light_ChangeToValue,
  Light_Fade,
  Light_Glow,
  Light_Flicker,
  Light_Strobe,

  Radius_Quake = 120,
  Line_SetIdentification,

  Thing_Move = 125,

  Thing_SetSpecial = 127,
  ThrustThingZ,
  UsePuzzleItem,
  Thing_Activate,
  Thing_Deactivate,
  Thing_Remove,
  Thing_Destroy,
  Thing_Projectile,
  Thing_Spawn,
  Thing_ProjectileGravity,
  Thing_SpawnNoFog,
  Floor_Waggle,

  Sector_ChangeSound = 140,

  MAX_LINE_SPAC,
} line_spac_t;

//
// Every sector action special
// https://doomwiki.org/wiki/Sector#Hexen
//

typedef enum {
  Light_Phased = 1,
  LightSequenceStart,
  LightSequenceSpecial1,
  LightSequenceSpecial2,

  Secret = 9,

  Stairs_Special1 = 26,
  Stairs_Special2,

  Wind_East_Weak = 40,
  Wind_East_Medium,
  Wind_East_Strong,
  Wind_North_Weak,
  Wind_North_Medium,
  Wind_North_Strong,
  Wind_South_Weak,
  Wind_South_Medium,
  Wind_South_Strong,
  Wind_West_Weak,
  Wind_West_Medium,
  Wind_West_Strong,

  Light_IndoorLightning1 = 198,
  Light_IndoorLightning2,
  Sky2,
  Scroll_North_Slow,
  Scroll_North_Medium,
  Scroll_North_Fast,
  Scroll_East_Slow,
  Scroll_East_Medium,
  Scroll_East_Fast,
  Scroll_South_Slow,
  Scroll_South_Medium,
  Scroll_South_Fast,
  Scroll_West_Slow,
  Scroll_West_Medium,
  Scroll_West_Fast,
  Scroll_NorthWest_Slow,
  Scroll_NorthWest_Medium,
  Scroll_NorthWest_Fast,
  Scroll_NorthEast_Slow,
  Scroll_NorthEast_Medium,
  Scroll_NorthEast_Fast,
  Scroll_SouthEast_Slow,
  Scroll_SouthEast_Medium,
  Scroll_SouthEast_Fast,
  Scroll_SouthWest_Slow,
  Scroll_SouthWest_Medium,
  Scroll_SouthWest_Fast,

  MAX_SECTOR_SPAC,
} sector_spac_t ;

#endif // __DOOMDATA__

//----------------------------------------------------------------------------
//
// $Log: doomdata.h,v $
// Revision 1.4  1998/05/03  22:39:10  killough
// beautification
//
// Revision 1.3  1998/03/23  06:42:57  jim
// linedefs reference initial version
//
// Revision 1.2  1998/01/26  19:26:37  phares
// First rev with no ^Ms
//
// Revision 1.1.1.1  1998/01/19  14:02:51  rand
// Lee's Jan 19 sources
//
//----------------------------------------------------------------------------
