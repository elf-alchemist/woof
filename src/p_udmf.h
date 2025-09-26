
#ifndef __P_UDMF__
#define __P_UDMF__

#include "p_extnodes.h"
//
// Universal Doom Map Format (UDMF) support
//

typedef enum {
  UDMF_LABEL,
  UDMF_TEXTMAP,
  UDMF_BLOCKMAP,
  UDMF_REJECT,
  UDMF_ZNODES,
  UDMF_ENDMAP,
} UDMF_Lumps_t;

typedef enum {
  UDMF_Invalid = -1,
  UDMF_Doom,
  UDMF_MBF21,
} UDMF_Namespace_t;

typedef struct {
  // Base spec
  int    id;
  int    type;
  double x;
  double y;
  double height;
  int    angle;
  int    options;
} UDMF_Thing_t;

typedef struct {
  // Base spec
  double x;
  double y;
} UDMF_Vertex_t;

typedef struct {
  // Base spec
  int id;
  int v1_id;
  int v2_id;
  int special;
  int sidefront;
  int sideback;
  int flags;

  // Woof!
  char tranmap[9];
} UDMF_Linedef_t;

typedef struct {
  // Base spec
  int sector_id;
  char texturetop[9];
  char texturemiddle[9];
  char texturebottom[9];
  double offsetx;
  double offsety;
} UDMF_Sidedef_t;

typedef struct {
  // Base spec
  int  tag;
  int  heightfloor;
  int  heightceiling;
  char texturefloor[9];
  char textureceiling[9];
  int  lightlevel;
  int  special;
} UDMF_Sector_t;

extern void UDMF_ParseTextMap(int lumpnum);
extern void UDMF_LoadMap(int lumpnum, nodeformat_t *nodeformat, int *gen_blockmap, int *pad_reject);
extern void UDMF_LoadThings(void);

#endif
