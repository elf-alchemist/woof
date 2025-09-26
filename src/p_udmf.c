


#include "doomdata.h"
#include "doomdef.h"
#include "doomstat.h"
#include "i_printf.h"
#include "i_system.h"
#include "m_argv.h"
#include "m_array.h"
#include "m_bbox.h"
#include "m_misc.h"
#include "m_scanner.h"
#include "m_swap.h"
#include "nano_bsp.h"
#include "p_extnodes.h"
#include "p_mobj.h"
#include "p_setup.h"
#include "p_udmf.h"
#include "r_data.h"
#include "r_main.h"
#include "r_state.h"
#include "w_wad.h"

//
// UDMF
//

static char * const UDMF_Lumps[] = {
  [UDMF_LABEL]    = "-",
  [UDMF_TEXTMAP]  = "TEXTMAP",
  [UDMF_BLOCKMAP] = "BLOCKMAP",
  [UDMF_REJECT]   = "REJECT",
  [UDMF_ZNODES]   = "ZNODES",
  [UDMF_ENDMAP]   = "ENDMAP",
};

static int udmf_namespace = UDMF_Invalid;

static int udmf_num_vertexes = 0;
static int udmf_num_linedefs = 0;
static int udmf_num_sidedefs = 0;
static int udmf_num_sectors = 0;
static int udmf_num_things = 0;

static UDMF_Vertex_t  *udmf_vertexes = NULL;
static UDMF_Linedef_t *udmf_linedefs = NULL;
static UDMF_Sidedef_t *udmf_sidedefs = NULL;
static UDMF_Sector_t  *udmf_sectors = NULL;
static UDMF_Thing_t   *udmf_things = NULL;

// All UDMF lumps in between TEXTMAP and ENDMAP
int UDMF_FindLump(int label_lumpnum, char * lumpname)
{
  int i = 0;
  for (i = UDMF_LABEL; i <= UDMF_ENDMAP; i++)
  {
    if (W_LumpExistsWithName(label_lumpnum + i, "ENDMAP"))
    {
      i = UDMF_LABEL; // Return label lumpnum, if lump not found
      break;
    }

    if (W_LumpExistsWithName(label_lumpnum + i, lumpname))
    {
      break;
    }
  }
  return label_lumpnum + i;
}

//
// UDMF parsing utils
//

// Retrieve plain integer
#define UDMF_ScanInt(s, x)         \
{                                  \
  SC_MustGetToken(s, '=');         \
  SC_MustGetToken(s, TK_IntConst); \
  x = SC_GetNumber(s);             \
  SC_MustGetToken(s, ';');         \
};

// Retrieve double, converted to fixed_t
#define UDMF_ScanDouble(s, x)        \
{                                    \
  SC_MustGetToken(s, '=');           \
  SC_MustGetToken(s, TK_FloatConst); \
  x = SC_GetDecimal(s);              \
  SC_MustGetToken(s, ';');           \
};

// Sets provided flag on, if true
#define UDMF_ScanFlag(s, x, f)      \
{                                   \
  SC_MustGetToken(s, '=');          \
  SC_MustGetToken(s, TK_BoolConst); \
  if (SC_GetBoolean(s)) x |= f;     \
  SC_MustGetToken(s, ';');          \
};

// Sets provided flag on, if value is false
// i.e NOTSINGLE, NOTDM, NOTCOOP, etc
#define UDMF_ScanFlagInverted(s, x, f) \
{                                      \
  SC_MustGetToken(s, '=');             \
  SC_MustGetToken(s, TK_BoolConst);    \
  if (!SC_GetBoolean(s)) x |= f;       \
  SC_MustGetToken(s, ';');             \
};

// Retrieve plain string
#define UDMF_ScanLumpName(s, x)       \
{                                     \
  SC_MustGetToken(s, '=');            \
  SC_MustGetToken(s, TK_StringConst); \
  M_CopyLumpName(x, SC_GetString(s)); \
  SC_MustGetToken(s, ';');            \
};

// Skip unknown keyword
static inline void UDMF_SkipScan(scanner_t *s)
{
  if (SC_CheckToken(s, '='))
  {
    while (SC_TokensLeft(s))
    {
      if (SC_CheckToken(s, ';'))
      {
        break;
      }

      SC_GetNextToken(s, true);
    }
    return;
  }

  SC_MustGetToken(s, '{');
  {
    int brace_count = 1;
    while (SC_TokensLeft(s))
    {
      if (SC_CheckToken(s, '}'))
      {
        --brace_count;
      }
      else if (SC_CheckToken(s, '{'))
      {
        ++brace_count;
      }
      if (!brace_count)
      {
        break;
      }
      SC_GetNextToken(s, true);
    }
    return;
  }
}

// UDMF namespace
static void UDMF_ParseNamespace(scanner_t *s)
{
  SC_MustGetToken(s, '=');
  SC_MustGetToken(s, TK_StringConst);
  const char * name = SC_GetString(s);

  if (!strcasecmp(name, "doom"))
  {
    udmf_namespace = UDMF_Doom;
  }
  else if (!strcasecmp(name, "mbf21"))
  {
    udmf_namespace = UDMF_MBF21;
  }
  else
  {
    I_Error("Unknown UDMF namespace.");
  }

  SC_MustGetToken(s, ';');
}

//
// UDMF vertex pasring
//

static void UDMF_ParseVertex(scanner_t *s)
{
  UDMF_Vertex_t vertex = {0};

  SC_MustGetToken(s, '{');
  while (!SC_CheckToken(s, '}'))
  {
    SC_MustGetToken(s, TK_Identifier);
    const char *prop = SC_GetString(s);
    if (!strcasecmp(prop, "x"))
    {
      UDMF_ScanDouble(s, vertex.x);
    }
    else if (!strcasecmp(prop, "y"))
    {
      UDMF_ScanDouble(s, vertex.y);
    }
    else
    {
      UDMF_SkipScan(s);
    }
  }

  udmf_num_vertexes++;
  array_push(udmf_vertexes, vertex);
}

//
// UDMF linedef loading
//

static void UDMF_ParseLinedef(scanner_t *s)
{
  UDMF_Linedef_t line = {0};
  line.sideback = -1;

  SC_MustGetToken(s, '{');
  while (!SC_CheckToken(s, '}'))
  {
    SC_MustGetToken(s, TK_Identifier);
    const char *prop = SC_GetString(s);
    if (!strcasecmp(prop, "v1"))
    {
      UDMF_ScanInt(s, line.v1_id);
    }
    else if (!strcasecmp(prop, "v2"))
    {
      UDMF_ScanInt(s, line.v2_id);
    }
    else if (!strcasecmp(prop, "special"))
    {
      UDMF_ScanInt(s, line.special);
    }
    else if (!strcasecmp(prop, "id"))
    {
      UDMF_ScanInt(s, line.id);
    }
    else if (!strcasecmp(prop, "sidefront"))
    {
      UDMF_ScanInt(s, line.sidefront);
    }
    else if (!strcasecmp(prop, "sideback"))
    {
      UDMF_ScanInt(s, line.sideback);
    }
    else if (!strcasecmp(prop, "tranmap"))
    {
      UDMF_ScanLumpName(s, line.tranmap);
    }
    else if (!strcasecmp(prop, "blocking"))
    {
      UDMF_ScanFlag(s, line.flags, ML_BLOCKING);
    }
    else if (!strcasecmp(prop, "blockmonsters"))
    {
      UDMF_ScanFlag(s, line.flags, ML_BLOCKMONSTERS);
    }
    else if (!strcasecmp(prop, "twosided"))
    {
      UDMF_ScanFlag(s, line.flags, ML_TWOSIDED);
    }
    else if (!strcasecmp(prop, "dontpegtop"))
    {
      UDMF_ScanFlag(s, line.flags, ML_DONTPEGTOP);
    }
    else if (!strcasecmp(prop, "dontpegbottom"))
    {
      UDMF_ScanFlag(s, line.flags, ML_DONTPEGBOTTOM);
    }
    else if (!strcasecmp(prop, "secret"))
    {
      UDMF_ScanFlag(s, line.flags, ML_SECRET);
    }
    else if (!strcasecmp(prop, "blocksound"))
    {
      UDMF_ScanFlag(s, line.flags, ML_SOUNDBLOCK);
    }
    else if (!strcasecmp(prop, "dontdraw"))
    {
      UDMF_ScanFlag(s, line.flags, ML_DONTDRAW);
    }
    else if (!strcasecmp(prop, "mapped"))
    {
      UDMF_ScanFlag(s, line.flags, ML_MAPPED);
    }
    else if (!strcasecmp(prop, "passuse")) // Boom
    {
      UDMF_ScanFlag(s, line.flags, ML_PASSUSE);
    }
    else if (!strcasecmp(prop, "blocklandmonsters")) // MBF21
    {
      UDMF_ScanFlag(s, line.flags, ML_BLOCKLANDMONSTERS);
    }
    else if (!strcasecmp(prop, "blockplayers")) // MBF21
    {
      UDMF_ScanFlag(s, line.flags, ML_BLOCKPLAYERS);
    }
    else
    {
      UDMF_SkipScan(s);
    }
  }

  udmf_num_linedefs++;
  array_push(udmf_linedefs, line);
}

//
// UDMF sidedef parsing
//

static void UDMF_ParseSidedef(scanner_t *s)
{
  UDMF_Sidedef_t side = {0};
  M_CopyLumpName(side.texturetop, "-");
  M_CopyLumpName(side.texturemiddle, "-");
  M_CopyLumpName(side.texturebottom, "-");

  SC_MustGetToken(s, '{');
  while (!SC_CheckToken(s, '}'))
  {
    SC_MustGetToken(s, TK_Identifier);
    const char *prop = SC_GetString(s);
    if (!strcasecmp(prop, "offsetx"))
    {
      UDMF_ScanDouble(s, side.offsetx);
    }
    else if (!strcasecmp(prop, "offsety"))
    {
      UDMF_ScanDouble(s, side.offsety);
    }
    else if (!strcasecmp(prop, "sector"))
    {
      UDMF_ScanInt(s, side.sector_id);
    }
    else if (!strcasecmp(prop, "texturetop"))
    {
      UDMF_ScanLumpName(s, side.texturetop);
    }
    else if (!strcasecmp(prop, "texturemiddle"))
    {
      UDMF_ScanLumpName(s, side.texturemiddle);
    }
    else if (!strcasecmp(prop, "texturebottom"))
    {
      UDMF_ScanLumpName(s, side.texturebottom);
    }
    else
    {
      UDMF_SkipScan(s);
    }
  }


  udmf_num_sidedefs++;
  array_push(udmf_sidedefs, side);
}

//
// UDMF sector parsing
//

static void UDMF_ParseSector(scanner_t *s)
{
  UDMF_Sector_t sector = {0};
  sector.lightlevel = 160;
  M_CopyLumpName(sector.texturefloor, "-");
  M_CopyLumpName(sector.textureceiling, "-");

  SC_MustGetToken(s, '{');
  while (!SC_CheckToken(s, '}'))
  {
    SC_MustGetToken(s, TK_Identifier);
    const char *prop = SC_GetString(s);
    if (!strcasecmp(prop, "heightfloor"))
    {
      UDMF_ScanInt(s, sector.heightfloor);
    }
    else if (!strcasecmp(prop, "heightceiling"))
    {
      UDMF_ScanInt(s, sector.heightceiling);
    }
    else if (!strcasecmp(prop, "texturefloor"))
    {
      UDMF_ScanLumpName(s, sector.texturefloor);
    }
    else if (!strcasecmp(prop, "textureceiling"))
    {
      UDMF_ScanLumpName(s, sector.textureceiling);
    }
    else if (!strcasecmp(prop, "lightlevel"))
    {
      UDMF_ScanInt(s, sector.lightlevel);
    }
    else if (!strcasecmp(prop, "id"))
    {
      UDMF_ScanInt(s, sector.tag);
    }
    else
    {
      UDMF_SkipScan(s);
    }
  }

  udmf_num_sectors++;
  array_push(udmf_sectors, sector);
}

//
// UDMF thing loading
//

static void UDMF_ParseThing(scanner_t *s)
{
  UDMF_Thing_t thing = {0};

  SC_MustGetToken(s, '{');
  while (!SC_CheckToken(s, '}'))
  {
    SC_MustGetToken(s, TK_Identifier);
    const char *prop = SC_GetString(s);
    if (!strcasecmp(prop, "type"))
    {
      UDMF_ScanInt(s, thing.type);
    }
    else if (!strcasecmp(prop, "x"))
    {
      UDMF_ScanDouble(s, thing.x);
    }
    else if (!strcasecmp(prop, "y"))
    {
      UDMF_ScanDouble(s, thing.y);
    }
    else if (!strcasecmp(prop, "height"))
    {
      UDMF_ScanDouble(s, thing.height);
    }
    else if (!strcasecmp(prop, "angle"))
    {
      UDMF_ScanInt(s, thing.angle);
    }
    else if (!strcasecmp(prop, "skill1"))
    {
      UDMF_ScanFlag(s, thing.options, MTF_SKILL1);
    }
    else if (!strcasecmp(prop, "skill2"))
    {
      UDMF_ScanFlag(s, thing.options, MTF_SKILL2);
    }
    else if (!strcasecmp(prop, "skill3"))
    {
      UDMF_ScanFlag(s, thing.options, MTF_SKILL3);
    }
    else if (!strcasecmp(prop, "skill4"))
    {
      UDMF_ScanFlag(s, thing.options, MTF_SKILL4);
    }
    else if (!strcasecmp(prop, "skill5"))
    {
      UDMF_ScanFlag(s, thing.options, MTF_SKILL5);
    }
    else if (!strcasecmp(prop, "ambush"))
    {
      UDMF_ScanFlag(s, thing.options, MTF_AMBUSH);
    }
    else if (!strcasecmp(prop, "single"))
    {
      UDMF_ScanFlagInverted(s, thing.options, MTF_NOTSINGLE);
    }
    else if (!strcasecmp(prop, "dm")) // Boom
    {
      UDMF_ScanFlagInverted(s, thing.options, MTF_NOTDM);
    }
    else if (!strcasecmp(prop, "coop")) // Boom
    {
      UDMF_ScanFlagInverted(s, thing.options, MTF_NOTCOOP);
    }
    else if (!strcasecmp(prop, "friend")) // MBF
    {
      UDMF_ScanFlag(s, thing.options, MTF_FRIEND);
    }
    else
    {
      UDMF_SkipScan(s);
    }
  }

  udmf_num_things++;
  array_push(udmf_things, thing);
}

//
// UDMF textmap loading
//

void UDMF_ParseTextMap(int lumpnum)
{
  scanner_t *s = SC_Open("TEXTMAP",
                         W_CacheLumpNum(lumpnum + UDMF_TEXTMAP, PU_LEVEL),
                         W_LumpLength(lumpnum + UDMF_TEXTMAP));

  const char* toplevel = NULL;
  while (SC_TokensLeft(s))
  {
    SC_MustGetToken(s, TK_Identifier);
    toplevel = SC_GetString(s);

    if (!strcasecmp(toplevel, "namespace"))
    {
      UDMF_ParseNamespace(s);
    }
    else if (!strcasecmp(toplevel, "vertex"))
    {
      UDMF_ParseVertex(s);
    }
    else if (!strcasecmp(toplevel, "linedef"))
    {
      UDMF_ParseLinedef(s);
    }
    else if (!strcasecmp(toplevel, "sidedef"))
    {
      UDMF_ParseSidedef(s);
    }
    else if (!strcasecmp(toplevel, "sector"))
    {
      UDMF_ParseSector(s);
    }
    else if (!strcasecmp(toplevel, "thing"))
    {
      UDMF_ParseThing(s);
    }
    else
    {
      UDMF_SkipScan(s);
    }
  }

  if (udmf_num_vertexes == 0)
  {
    I_Printf(VB_WARNING, "Not enough UDMF vertexes");
  }

  if (udmf_num_linedefs == 0)
  {
    I_Printf(VB_WARNING, "Not enough UDMF linedefs");
  }

  if (udmf_num_sidedefs == 0)
  {
    I_Printf(VB_WARNING, "Not enough UDMF sidedefs");
  }

  if (udmf_num_sectors == 0)
  {
    I_Printf(VB_WARNING, "Not enough UDMF sectors");
  }

  if (udmf_num_things == 0)
  {
    I_Printf(VB_WARNING, "Not enough UDMF things");
  }

  if (udmf_num_vertexes == 0
      || udmf_num_linedefs == 0
      || udmf_num_sidedefs == 0
      || udmf_num_sectors == 0
      || udmf_num_things == 0)
  {
    SC_Error(s, "Not enough UDMF data. Check your TEXTMAP.");
  }

  SC_Close(s);
}

static void UDMF_LoadVertexes(void)
{
  numvertexes = udmf_num_vertexes;
  vertexes = Z_Malloc(numvertexes * sizeof(vertex_t), PU_LEVEL, 0);
  memset(vertexes, 0, numvertexes * sizeof(vertex_t));

  for (int i = 0; i < numvertexes; i++)
  {
    vertexes[i].x = DoubleToFixed(udmf_vertexes[i].x);
    vertexes[i].y = DoubleToFixed(udmf_vertexes[i].x);

    vertexes[i].r_x = vertexes[i].x;
    vertexes[i].r_y = vertexes[i].y;
  }
}

static void UDMF_LoadSectors(void)
{
  numsectors = udmf_num_sectors;
  sectors = Z_Malloc(numsectors * sizeof(sector_t), PU_LEVEL, 0);
  memset(sectors, 0, numsectors * sizeof(sector_t));

  for (int i = 0; i < numsectors; i++)
  {
    sectors[i].floorheight = udmf_sectors[i].heightfloor << FRACBITS;
    sectors[i].ceilingheight = udmf_sectors[i].heightceiling << FRACBITS;
    sectors[i].floorpic = R_FlatNumForName(udmf_sectors[i].texturefloor);
    sectors[i].ceilingpic = R_FlatNumForName(udmf_sectors[i].textureceiling);
    sectors[i].lightlevel = udmf_sectors[i].lightlevel;
    sectors[i].tag = udmf_sectors[i].tag;
 
    sectors[i].thinglist = NULL;
    sectors[i].touching_thinglist = NULL; // phares 3/14/98

    sectors[i].nextsec = -1; //jff 2/26/98 add fields to support locking out
    sectors[i].prevsec = -1; // stair retriggering until build completes

    sectors[i].heightsec = -1;       // sector used to get floor and ceiling height
    sectors[i].floorlightsec = -1;   // sector used to get floor lighting
    sectors[i].ceilinglightsec = -1; // sector used to get ceiling lighting:

    // [AM] Sector interpolation.  Even if we're
    //    not running uncapped, the renderer still
    //    uses this data.
    sectors[i].oldfloorheight = sectors[i].interpfloorheight = sectors[i].floorheight;
    sectors[i].oldceilingheight = sectors[i].interpceilingheight = sectors[i].ceilingheight;

    // [FG] inhibit sector interpolation during the 0th gametic
    sectors[i].oldceilgametic = sectors[i].oldfloorgametic = -1;
    sectors[i].old_ceil_offs_gametic = sectors[i].old_floor_offs_gametic = -1;
  }
}

static void UDMF_LoadSideDefs(void)
{
  numsides = udmf_num_sidedefs;
  sides = Z_Malloc(numsides * sizeof(side_t), PU_LEVEL, 0);
  memset(sides, 0, numsides * sizeof(side_t));

  for (int i = 0; i < numsides; i++)
  {
    sides[i].sector = &sectors[udmf_sidedefs[i].sector_id];
    sides[i].textureoffset = DoubleToFixed(udmf_sidedefs[i].offsetx);
    sides[i].rowoffset = DoubleToFixed(udmf_sidedefs[i].offsety);

    // [crispy] smooth texture scrolling
    sides[i].oldtextureoffset = sides[i].interptextureoffset = sides[i].textureoffset;
    sides[i].oldrowoffset = sides[i].interprowoffset = sides[i].rowoffset;
    sides[i].oldgametic = -1;

    sides[i].toptexture = R_TextureNumForName(udmf_sidedefs[i].texturetop);
    sides[i].midtexture = R_TextureNumForName(udmf_sidedefs[i].texturemiddle);
    sides[i].bottomtexture = R_TextureNumForName(udmf_sidedefs[i].texturebottom);
  }
}

static void UDMF_LoadLineDefs(void)
{
  numlines = udmf_num_linedefs;
  lines = Z_Malloc(numlines * sizeof(line_t), PU_LEVEL, 0);
  memset(lines, 0, numlines * sizeof(line_t));

  vertex_t *v1, *v2;

  for (int i = 0; i < numlines; i++)
  {
    lines[i].v1 = v1 = &vertexes[udmf_linedefs[i].v1_id];
    lines[i].v2 = v2 = &vertexes[udmf_linedefs[i].v2_id];
    lines[i].sidenum[0] = udmf_linedefs[i].sidefront;
    lines[i].sidenum[1] = udmf_linedefs[i].sideback;

    lines[i].flags = udmf_linedefs[i].flags;
    lines[i].tag   = udmf_linedefs[i].id;

    if (strcasecmp(udmf_linedefs[i].tranmap, "-"))
    {
      // Line has TRANMAP lump
      if (!strcasecmp(udmf_linedefs[i].tranmap, "TRANMAP"))
      {
        // Is using default built-in TRANMAP lump
        lines[i].tranlump = 0;
      }
      else
      {
        lines[i].tranlump = W_CheckNumForName(udmf_linedefs[i].tranmap);
        if (lines[i].tranlump < 0 || W_LumpLength(lines[i].tranlump) != 65536)
        {
          // Is using improper or non-existent custom TRANMAP lump
          lines[i].tranlump = 0;
        }
        else
        {
          // Is using proper custom TRANMAP lump
          W_CacheLumpNum(lines[i].tranlump, PU_CACHE);
          lines[i].tranlump++;
        }
      }
    }

    lines[i].dx = v2->x - v1->x;
    lines[i].dy = v2->y - v1->y;
    lines[i].angle = R_PointToAngle2(v1->x, v1->y, v2->x, v2->y);

    lines[i].slopetype = !lines[i].dx                           ? ST_VERTICAL
                       : !lines[i].dy                           ? ST_HORIZONTAL
                       : FixedDiv(lines[i].dy, lines[i].dx) > 0 ? ST_POSITIVE
                                                                : ST_NEGATIVE;

    if (v1->x < v2->x)
    {
      lines[i].bbox[BOXLEFT] = v1->x;
      lines[i].bbox[BOXRIGHT] = v2->x;
    }
    else
    {
      lines[i].bbox[BOXLEFT] = v2->x;
      lines[i].bbox[BOXRIGHT] = v1->x;
    }

    if (v1->y < v2->y)
    {
      lines[i].bbox[BOXBOTTOM] = v1->y;
      lines[i].bbox[BOXTOP] = v2->y;
    }
    else
    {
      lines[i].bbox[BOXBOTTOM] = v2->y;
      lines[i].bbox[BOXTOP] = v1->y;
    }

    // killough 11/98: fix common wad errors (missing sidedefs):
    // Substitute dummy sidedef for missing right side
    if (lines[i].sidenum[0] == NO_INDEX)
    {
      lines[i].sidenum[0] = 0;
    }

    // Clear 2s flag for missing left side
    if (lines[i].sidenum[1] == NO_INDEX && (!demo_compatibility || !overflow[emu_missedbackside].enabled))
    {
      lines[i].flags &= ~ML_TWOSIDED;
    }

    if (lines[i].sidenum[0] != NO_INDEX)
    {
      lines[i].frontsector = sides[lines[i].sidenum[0]].sector;
    }

    if (lines[i].sidenum[1] != NO_INDEX)
    {
      lines[i].backsector = sides[lines[i].sidenum[1]].sector;
    }
  }
}

void UDMF_LoadThings(void)
{
  for (int i = 0; i < udmf_num_things; i++)
  {
    // Do not spawn cool, new monsters if !commercial
    if (gamemode != commercial)
    {
      switch(udmf_things[i].type)
        {
        case 68:  // Arachnotron
        case 64:  // Archvile
        case 88:  // Boss Brain
        case 89:  // Boss Shooter
        case 69:  // Hell Knight
        case 67:  // Mancubus
        case 71:  // Pain Elemental
        case 65:  // Former Human Commando
        case 66:  // Revenant
        case 84:  // Wolf SS
          continue;
        }
    }

    // Do spawn all other stuff.

    mapthing_t mt = {0};
    mt.x = DoubleToFixed(udmf_things[i].x);
    mt.y = DoubleToFixed(udmf_things[i].y);
    mt.height = DoubleToFixed(udmf_things[i].height);
    mt.angle = udmf_things[i].angle;
    mt.type = udmf_things[i].type;
    mt.options = udmf_things[i].options;

    P_SpawnMapThing(&mt);
  }
}

nodeformat_t UDMF_LoadNodes(int label_num)
{
  nodeformat_t nodeformat = NFMT_NANO;
  int znodes_num = UDMF_FindLump(label_num, UDMF_Lumps[UDMF_ZNODES]);

  if (W_LumpExistsWithName(znodes_num, "ZNODES"))
  {
    P_CheckUDMFNodeFormat(znodes_num);
  }

  // [FG] support maps with NODES in uncompressed XNOD/XGLN or compressed ZNOD/ZGLN formats, or DeePBSP format
  if (nodeformat >= NFMT_XNOD && nodeformat <= NFMT_ZGL3)
  {
    P_LoadNodes_ZDoom(znodes_num, nodeformat);
  }
  else
  {
    BSP_BuildNodes();
  }

  return nodeformat;
}

boolean UDMF_LoadBlockMap(int label_num)
{
  long count;
  boolean ret = true;
  int lumpnum = UDMF_FindLump(label_num, UDMF_Lumps[UDMF_BLOCKMAP]);

  //!
  // @category mod
  //
  // Forces a (re-)building of the BLOCKMAP lumps for loaded maps.
  //

  // [FG] always rebuild too short blockmaps
  if (M_CheckParm("-blockmap") || (count = W_LumpLengthWithName(lumpnum, "BLOCKMAP")/2) >= 0x10000 || count < 4)
  {
    P_CreateBlockMap();
  }
  else
  {
    long i;
    short *wadblockmaplump = W_CacheLumpNum(lumpnum, PU_LEVEL);
    blockmaplump = Z_Malloc(sizeof(*blockmaplump) * count, PU_LEVEL, 0);

    // killough 3/1/98: Expand wad blockmap into larger internal one,
    // by treating all offsets except -1 as unsigned and zero-extending
    // them. This potentially doubles the size of blockmaps allowed,
    // because Doom originally considered the offsets as always signed.

    blockmaplump[0] = SHORT(wadblockmaplump[0]);
    blockmaplump[1] = SHORT(wadblockmaplump[1]);
    blockmaplump[2] = (long)(SHORT(wadblockmaplump[2])) & FRACMASK;
    blockmaplump[3] = (long)(SHORT(wadblockmaplump[3])) & FRACMASK;

    for (i=4 ; i < count ; i++)
    {
      short t = SHORT(wadblockmaplump[i]); // killough 3/1/98
      blockmaplump[i] = t == -1 ? -1l : (long) t & FRACMASK;
    }

    Z_Free(wadblockmaplump);

    bmaporgx = blockmaplump[0]<<FRACBITS;
    bmaporgy = blockmaplump[1]<<FRACBITS;
    bmapwidth = blockmaplump[2];
    bmapheight = blockmaplump[3];

    ret = false;

    P_SetSkipBlockStart();
  }

  // clear out mobj chains
  blocklinks_size = sizeof(*blocklinks) * bmapwidth * bmapheight;
  blocklinks = Z_Malloc(blocklinks_size, PU_LEVEL, 0);
  memset(blocklinks, 0, blocklinks_size);
  blockmap = blockmaplump + 4;

  return ret;
}

boolean UDMF_LoadReject(int label_num, int totallines)
{
    // Calculate the size that the REJECT lump *should* be.
    int minlength = (numsectors * numsectors + 7) / 8;
    int lumpnum = UDMF_FindLump(label_num, UDMF_Lumps[UDMF_REJECT]);
    int lumplen = W_LumpLengthWithName(lumpnum, "REJECT");
    boolean ret;

    // If the lump meets the minimum length, it can be loaded directly.
    // Otherwise, we need to allocate a buffer of the correct size
    // and pad it with appropriate data.

    if (lumplen >= minlength)
    {
        rejectmatrix = W_CacheLumpNum(lumpnum, PU_LEVEL);
        ret = false;
    }
    else
    {
        unsigned int padvalue = 0x00;

        rejectmatrix = Z_Malloc(minlength, PU_LEVEL, (void **) &rejectmatrix);
        W_ReadLump(lumpnum, rejectmatrix);

        if (M_CheckParm("-reject_pad_with_ff"))
        {
            padvalue = 0xff;
        }

        memset(rejectmatrix + lumplen, padvalue, minlength - lumplen);

        if (demo_compatibility && overflow[emu_reject].enabled)
        {
            unsigned int i;
            unsigned int byte_num;
            byte *dest;

            unsigned int rejectpad[4] =
            {
                0,                               // Size
                0,                               // Part of z_zone block header
                50,                              // PU_LEVEL
                0x1d4a11                         // DOOM_CONST_ZONEID
            };

            overflow[emu_reject].triggered = true;

            rejectpad[0] = ((totallines * 4 + 3) & ~3) + 24;

            // Copy values from rejectpad into the destination array.

            dest = rejectmatrix + lumplen;

            for (i = 0; i < (minlength - lumplen) && i < sizeof(rejectpad); ++i)
            {
                byte_num = i % 4;
                *dest = (rejectpad[i / 4] >> (byte_num * 8)) & 0xff;
                ++dest;
            }
        }

        ret = true;
    }

    return ret;
}

void UDMF_LoadMap(int lumpnum, nodeformat_t *nodeformat, int *gen_blockmap, int *pad_reject)
{
  // note: most of this ordering is important

  UDMF_LoadVertexes();
  UDMF_LoadSectors();
  UDMF_LoadSideDefs(); // <- This needs Sectors
  UDMF_LoadLineDefs(); // <- this needs Sides

  
  *gen_blockmap = UDMF_LoadBlockMap(lumpnum);
  *nodeformat = UDMF_LoadNodes(lumpnum);
  *pad_reject = UDMF_LoadReject(lumpnum, P_GroupLines());
}
