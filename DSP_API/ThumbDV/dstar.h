///*!   \file dstar.h
// *
// *    Handles all DSTAR States
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

#include "bit_pattern_matcher.h"
#include "DStarDefines.h"
#include "dstar.h"

enum _slow_data_decode_state {
    FIRST_FRAME = 0,
    HEADER_SECOND_FRAME,
    MESSAGE_SECOND_FRAME
};

typedef struct _slow_data_decoder {
    enum _slow_data_decode_state decode_state;
    unsigned char header_bytes[RADIO_HEADER_LENGTH_BYTES];
    uint32 header_array_index;
    unsigned char message[4][6];
    uint32 frame_count;
} slow_data_decoder, * SLOW_DATA_DECODER;

typedef struct _slow_data_machine {

} slow_data_machine, *SLOW_DATA_MACHINE;


enum DSTAR_STATE {
    BIT_FRAME_SYNC_WAIT = 0x1,
    HEADER_PROCESSING,
    VOICE_FRAME,
    DATA_FRAME,
    DATA_SYNC_FRAME,
    END_PATTERN_FOUND
};

enum STATUS_TYPE {
    STATUS_RX = 0,
    STATUS_TX
};

typedef struct _dstar_header {
    unsigned char flag1;
    unsigned char flag2;
    unsigned char flag3;
    char destination_rptr[9];
    char departure_rptr[9];
    char companion_call[9];
    char own_call1[9];
    char own_call2[5];
    uint16  p_fcs;
} dstar_header, * DSTAR_HEADER;

typedef struct _dstar_machine {
    enum DSTAR_STATE state;
    dstar_header incoming_header;
    dstar_header outgoing_header;

    uint32 bit_count;
    uint32 frame_count;

    /* BIT Pattern Matcher */
    BIT_PM  syn_pm;
    BIT_PM  data_sync_pm;
    BIT_PM  end_pm;

    /* Bit Buffers */
    BOOL header[FEC_SECTION_LENGTH_BITS];
    BOOL voice_bits[VOICE_FRAME_LENGTH_BITS];
    BOOL data_bits[DATA_FRAME_LENGTH_BITS];

    SLOW_DATA_DECODER slow_decoder;

    uint32 slice;

} dstar_machine, * DSTAR_MACHINE;

typedef struct _dstar_fec {
    BOOL mem0[RADIO_HEADER_LENGTH_BITS];
    BOOL mem1[RADIO_HEADER_LENGTH_BITS];
    BOOL mem2[RADIO_HEADER_LENGTH_BITS];
    BOOL mem3[RADIO_HEADER_LENGTH_BITS];
    int metric[4];
} dstar_fec, * DSTAR_FEC;

typedef union _dstar_pfcs {
    uint16 crc16;
    uint8 crc8[2];
} dstar_pfcs, * DSTAR_PFCS;

void dstar_updateStatus( DSTAR_MACHINE machine, uint32 slice , enum STATUS_TYPE type );
DSTAR_MACHINE dstar_createMachine( void );
void dstar_destroyMachine( DSTAR_MACHINE machine );
BOOL dstar_stateMachine( DSTAR_MACHINE machine, BOOL in_bit, unsigned char * ambe_out, uint32 ambe_buf_len );

void dstar_dumpHeader( DSTAR_HEADER header );

void dstar_processHeader( unsigned char * bytes, DSTAR_HEADER header );
void dstar_pfcsUpdate( DSTAR_PFCS pfcs, BOOL * bits );
BOOL dstar_pfcsCheck( DSTAR_PFCS pfcs, BOOL * bits );
void dstar_pfcsResult( DSTAR_PFCS pfcs, unsigned char * chksum );
void dstar_pfcsResultBits( DSTAR_PFCS pfcs, BOOL * bits );
void dstar_pfcsUpdateBuffer( DSTAR_PFCS pfcs, unsigned char * bytes, uint32 length );
void dstar_headerToBytes( DSTAR_HEADER header, unsigned char * bytes );

void dstar_FECTest( void );
void dstar_scramble( BOOL * in, BOOL * out, uint32 length, uint32 * scramble_count );
void dstar_interleave( const BOOL * in, BOOL * out, unsigned int length );
void dstar_deinterleave( const BOOL * in, BOOL * out, unsigned int length );
BOOL dstar_FECdecode( DSTAR_FEC fec, const BOOL * in, BOOL * out, unsigned int inLen, unsigned int * outLen );
void dstar_FECencode( const BOOL * in, BOOL * out, unsigned int inLen, unsigned int * outLen );

#endif /* THUMBDV_DSTAR_H_ */

