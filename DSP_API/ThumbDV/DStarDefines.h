/*
 *  Copyright (C) 2009,2012 by Jonathan Naylor, G4KLX
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  Modified for FlexRadio WaveformAPI by Ed Gonzalez KG5FBT
 */

/* THUMBDV_DSTARDEFINES_H_ */
#ifndef THUMBDV_DSTARDEFINES_H_
#define THUMBDV_DSTARDEFINES_H_

#include "datatypes.h"

const unsigned int DSTAR_GMSK_SYMBOL_RATE = 4800U;
const float        DSTAR_GMSK_BT          = 0.5F;

const BOOL BIT_SYNC_BITS[]    = {TRUE, FALSE, TRUE, FALSE};
const unsigned int BIT_SYNC_LENGTH_BITS = 4U;

const BOOL FRAME_SYNC_BITS[]  = {TRUE, TRUE,  TRUE, FALSE, TRUE,  TRUE,  FALSE, FALSE,
                                 TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE};
const unsigned int FRAME_SYNC_LENGTH_BITS = 15U;

const unsigned char DATA_SYNC_BYTES[] = {0x55, 0x2D, 0x16};
const BOOL DATA_SYNC_BITS[]   = {TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE,
                                 TRUE,  FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE,
                                 FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE};

const unsigned char END_PATTERN_BYTES[] = {0x55, 0x55, 0x55, 0x55, 0xC8, 0x7A};
const BOOL END_PATTERN_BITS[] = {TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE,
                                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE,
                                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE,
                                 TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE,
                                 FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,
                                 FALSE, TRUE,  FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE};
const unsigned int END_PATTERN_LENGTH_BITS  = 48U;
const unsigned int END_PATTERN_LENGTH_BYTES = END_PATTERN_LENGTH_BITS / 8U;

const unsigned char NULL_AMBE_DATA_BYTES[] = {0x9E, 0x8D, 0x32, 0x88, 0x26, 0x1A, 0x3F, 0x61, 0xE8};
const BOOL NULL_AMBE_DATA_BITS[] = {FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,
                                    TRUE,  FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,
                                    FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE,
                                    FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,
                                    FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE,
                                    FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE,
                                    TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE,
                                    TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE,
                                    FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  TRUE};

// Note that these are already scrambled, 0x66 0x66 0x66 otherwise
const BOOL NULL_SLOW_DATA_BITS[] = {FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE,
                                    FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, TRUE,
                                    TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, TRUE};

const unsigned int VOICE_FRAME_LENGTH_BITS  = 72U;
const unsigned int VOICE_FRAME_LENGTH_BYTES = VOICE_FRAME_LENGTH_BITS / 8U;

const unsigned int DATA_FRAME_LENGTH_BITS   = 24U;
const unsigned int DATA_FRAME_LENGTH_BYTES  = DATA_FRAME_LENGTH_BITS / 8U;

const unsigned int DV_FRAME_LENGTH_BITS     = VOICE_FRAME_LENGTH_BITS + DATA_FRAME_LENGTH_BITS;
const unsigned int DV_FRAME_LENGTH_BYTES    = VOICE_FRAME_LENGTH_BYTES + DATA_FRAME_LENGTH_BYTES;

const unsigned int FEC_SECTION_LENGTH_BITS  = 660U;

const unsigned int RADIO_HEADER_LENGTH_BITS  = 330U;
const unsigned int RADIO_HEADER_LENGTH_BYTES = 41U;

const unsigned int DATA_BLOCK_SIZE_BITS  = 21U * DV_FRAME_LENGTH_BITS;
const unsigned int DATA_BLOCK_SIZE_BYTES = 21U * DV_FRAME_LENGTH_BYTES;

const unsigned char SLOW_DATA_TYPE_MASK    = 0xF0;
const unsigned char SLOW_DATA_TYPE_GPSDATA = 0x30;
const unsigned char SLOW_DATA_TYPE_MESSAGE = 0x40;
const unsigned char SLOW_DATA_TYPE_HEADER  = 0x50;
const unsigned char SLOW_DATA_TYPE_SQUELCH = 0xC0;
const unsigned char SLOW_DATA_LENGTH_MASK  = 0x0F;

const unsigned int DSTAR_AUDIO_BLOCK_SIZE  = 160U;
const unsigned int DSTAR_AUDIO_BLOCK_BYTES = DSTAR_AUDIO_BLOCK_SIZE * 2U;

const unsigned int DSTAR_RADIO_SAMPLE_RATE = 48000U;
const unsigned int DSTAR_RADIO_BLOCK_SIZE  = 960U;

const unsigned int LONG_CALLSIGN_LENGTH  = 8U;
const unsigned int SHORT_CALLSIGN_LENGTH = 4U;

const unsigned int MESSAGE_LENGTH        = 20U;

const unsigned char DATA_MASK           = 0x80U;
const unsigned char REPEATER_MASK       = 0x40U;
const unsigned char INTERRUPTED_MASK    = 0x20U;
const unsigned char CONTROL_SIGNAL_MASK = 0x10U;
const unsigned char URGENT_MASK         = 0x08U;

const unsigned char REPEATER_CONTROL_MASK = 0x07U;
const unsigned char REPEATER_CONTROL      = 0x07U;
const unsigned char AUTO_REPLY            = 0x06U;
const unsigned char RESEND_REQUESTED      = 0x04U;
const unsigned char ACK_FLAG              = 0x03U;
const unsigned char NO_RESPONSE           = 0x02U;
const unsigned char RELAY_UNAVAILABLE     = 0x01U;

const unsigned int  DSTAR_BLEEP_FREQ      = 2000U;
const unsigned int  DSTAR_BLEEP_LENGTH    = 100U;
const float         DSTAR_BLEEP_AMPL      = 0.5F;

const unsigned int  DSTAR_RADIO_BIT_LENGTH = DSTAR_RADIO_SAMPLE_RATE / DSTAR_GMSK_SYMBOL_RATE;

const unsigned int  FRAME_TIME_MS = 20U;

const unsigned int  FRAMES_BETWEEN_SYNC = 20U;

const unsigned int  TICKS_PER_SEC = 50U;

#endif
