
#ifndef __P_UDMF__
#define __P_UDMF__

#include "p_extnodes.h"

extern void UDMF_ParseTextMap(int lumpnum);
extern void UDMF_LoadMap(int lumpnum, nodeformat_t nodeformat, int *gen_blockmap, int *pad_reject);
extern void UDMF_LoadThings(void);

#endif
