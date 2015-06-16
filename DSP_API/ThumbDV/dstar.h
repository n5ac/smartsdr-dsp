///*!   \file dstar.h
// *
// *    Handles scrambling and descrambling of DSTAR Header
// *
// *    \date 02-JUN-2015
// *    \author Ed Gonzalez KG5FBT modified from original in OpenDV code(C) 2009 Jonathan Naylor, G4KLX
// */

/* *****************************************************************************
 *
 *  Copyright (C) 2012-2014 FlexRadio Systems.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Contact Information:
 *  email: gpl<at>flexradiosystems.com
 *  Mail:  FlexRadio Systems, Suite 1-150, 4616 W. Howard LN, Austin, TX 78728
 *
 * ************************************************************************** */

#ifndef THUMBDV_DSTAR_H_
#define THUMBDV_DSTAR_H_

#include "DStarDefines.h"

enum DSTAR_STATE
{
    BIT_FRAME_SYNC_WAIT = 0x1,
    HEADER_PROCESSING,
    VOICE_FRAME,
    DATA_FRAME,
    DATA_SYNC_FRAME,
    END_PATTERN_FOUND
};

typedef struct _dstar_header
{
    unsigned char flag1;
    unsigned char flag2;
    unsigned char flag3;
    unsigned char destination_rptr[9];
    unsigned char departure_rptr[9];
    unsigned char companion_call[9];
    unsigned char own_call1[9];
    unsigned char own_call2[5];
    uint16  p_fcs;
} dstar_header, * DSTAR_HEADER;

typedef struct _dstar_machine
{
    enum DSTAR_STATE state;
    dstar_header incoming_header;
    dstar_header outgoing_header;

    uint32 bit_count;
    uint32 frame_count;

    BIT_PM  syn_pm;
    BIT_PM  data_sync_pm;
    BIT_PM  end_pm;

} dstar_machine, * DSTAR_MACHINE;

typedef struct _dstar_fec
{
    BOOL mem0[330];
    BOOL mem1[330];
    BOOL mem2[330];
    BOOL mem3[330];
    int metric[4];
} dstar_fec, * DSTAR_FEC;


DSTAR_MACHINE dstar_createMachine(void);
void dstar_destroyMachine(DSTAR_MACHINE machine);
BOOL dstar_stateMachine(DSTAR_MACHINE machine, BOOL in_bit, unsigned char * ambe_out, uint32 ambe_buf_len);



void dstar_FECTest(void);
void dstar_scramble(BOOL * in, BOOL * out, uint32 length, uint32 * scramble_count);
void dstar_interleave(const BOOL * in, BOOL * out, unsigned int length);
void dstar_deinterleave(const BOOL * in, BOOL * out, unsigned int length);
BOOL dstar_FECdecode(DSTAR_FEC fec, const BOOL * in, BOOL * out, unsigned int inLen, unsigned int * outLen);
#endif /* THUMBDV_DSTAR_H_ */
