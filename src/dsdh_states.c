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

#include "doomdef.h"
#include "doomstat.h"
#include "info.h"
#include "m_array.h"
#include "m_hashmap.h"

state_t *states = NULL;
int num_states;

static hashmap_t *translate;

int DSDH_StateTranslate(int frame_number);
int DSDH_SpriteTranslate(int sprite_number);

void DSDH_StatesInit(void)
{
    num_states = NUMSTATES;

    array_resize(states, NUMSTATES);
    memcpy(states, original_states, NUMSTATES * sizeof(*states));

    if (demo_version < DV_ID24)
    {
        return;
    }

    // TODO: wtf?
    for (int i = 0; i < NUMSTATES_ID24; i++)
    {
        int32_t real_index = DSDH_StateTranslate(ID24_NEG_OFFSET + i);
        states[real_index].sprite = DSDH_SpriteTranslate(id24_states[i].sprite);
        states[real_index].frame = DSDH_StateTranslate(id24_states[i].frame);
        states[real_index].tics = id24_states[i].tics;
        states[real_index].action = id24_states[i].action;
        states[real_index].nextstate = DSDH_StateTranslate(id24_states[i].nextstate);
        states[real_index].misc1 = id24_states[i].misc1;
        states[real_index].flags = id24_states[i].flags;
        states[real_index].args[0] = id24_states[i].args[0];
        states[real_index].args[1] = id24_states[i].args[1];
        states[real_index].args[2] = id24_states[i].args[2];
        states[real_index].args[3] = id24_states[i].args[3];
        states[real_index].args[4] = id24_states[i].args[4];
        states[real_index].args[5] = id24_states[i].args[5];
        states[real_index].args[6] = id24_states[i].args[6];
        states[real_index].args[7] = id24_states[i].args[7];
        states[real_index].tranmap = id24_states[i].tranmap;
    }
}

int DSDH_StateTranslate(int frame_number)
{
    if (frame_number < NUMSTATES)
    {
        return frame_number;
    }

    if (!translate)
    {
        translate = hashmap_init(2048);
    }

    int index;
    if (hashmap_get(translate, frame_number, &index))
    {
        return index;
    }

    index = num_states;
    hashmap_put(translate, frame_number, &index);

    state_t state = {.sprite = SPR_TNT1, .tics = -1, .nextstate = index};
    array_push(states, state);
    ++num_states;

    return index;
}
