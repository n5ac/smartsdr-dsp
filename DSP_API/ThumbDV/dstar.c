///*!   \file dstar.h
// *
// *    Handles scrambling and descrambling of DSTAR Header
// *
// *    \date 02-JUN-2015
// *    \author Ed Gonzalez KG5FBT modified from original in OpenDV code (C) 2009 Jonathan Naylor, G4KLX
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "DStarDefines.h"
#include "gmsk_modem.h"
#include "bit_pattern_matcher.h"
#include "thumbDV.h"
#include "dstar.h"

#define SCRAMBLER_TABLE_BITS_LENGTH     720U
#define SCRAMBLER_TABLE_BYTES_LENGTH    90U

static const BOOL SCRAMBLER_TABLE_BITS[] = {
    FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE,
    TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE,
    FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  TRUE,  FALSE,
    TRUE,  FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE,
    TRUE,  TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,
    TRUE,  FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE,
    TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,
    TRUE,  FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE,
    FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,
    TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE,
    FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE,
    FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,
    TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,
    FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,
    TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,
    FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE,
    FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,
    FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE,
    TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE,
    TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,
    FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE,
    TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,
    TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE,
    TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE,
    FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE,
    FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,
    FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, TRUE,
    TRUE,  FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE,
    TRUE,  FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,
    TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  TRUE,
    TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,
    TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE,
    TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE, FALSE,
    TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE,
    FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,
    FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,
    FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,
    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  TRUE,  TRUE,
    TRUE,  FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,
    TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,
    TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,
    FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE,
    TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE,
    TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE,
    TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE};



static const unsigned char SCRAMBLER_TABLE_BYTES[] = {
    0x0e, 0xf2, 0xc9, 0x02, 0x26, 0x2e, 0xb6, 0x0c, 0xd4, 0xe7, 0xb4, 0x2a, 0xfa, 0x51, 0xb8, 0xfe,
    0x1d, 0xe5, 0x92, 0x04, 0x4c, 0x5d, 0x6c, 0x19, 0xa9, 0xcf, 0x68, 0x55, 0xf4, 0xa3, 0x71, 0xfc,
    0x3b, 0xcb, 0x24, 0x08, 0x98, 0xba, 0xd8, 0x33, 0x53, 0x9e, 0xd0, 0xab, 0xe9, 0x46, 0xe3, 0xf8,
    0x77, 0x96, 0x48, 0x11, 0x31, 0x75, 0xb0, 0x66, 0xa7, 0x3d, 0xa1, 0x57, 0xd2, 0x8d, 0xc7, 0xf0,
    0xef, 0x2c, 0x90, 0x22, 0x62, 0xeb, 0x60, 0xcd, 0x4e, 0x7b, 0x42, 0xaf, 0xa5, 0x1b, 0x8f, 0xe1,
    0xde, 0x59, 0x20, 0x44, 0xc5, 0xd6, 0xc1, 0x9a, 0x9c, 0xf6};


static const unsigned int INTERLEAVE_TABLE[] = {
    0, 28, 56, 84, 112, 140, 168, 196, 224, 252, 280, 308, 336, 363, 390, 417, 444,
    471, 498, 525, 552, 579, 606, 633, 1, 29, 57, 85, 113, 141, 169, 197, 225, 253,
    281, 309, 337, 364, 391, 418, 445, 472, 499, 526, 553, 580, 607, 634, 2, 30, 58,
    86, 114, 142, 170, 198, 226, 254, 282, 310, 338, 365, 392, 419, 446, 473, 500,
    527, 554, 581, 608, 635, 3, 31, 59, 87, 115, 143, 171, 199, 227, 255, 283, 311,
    339, 366, 393, 420, 447, 474, 501, 528, 555, 582, 609, 636, 4, 32, 60, 88, 116,
    144, 172, 200, 228, 256, 284, 312, 340, 367, 394, 421, 448, 475, 502, 529, 556,
    583, 610, 637, 5, 33, 61, 89, 117, 145, 173, 201, 229, 257, 285, 313, 341, 368,
    395, 422, 449, 476, 503, 530, 557, 584, 611, 638, 6, 34, 62, 90, 118, 146, 174,
    202, 230, 258, 286, 314, 342, 369, 396, 423, 450, 477, 504, 531, 558, 585, 612,
    639, 7, 35, 63, 91, 119, 147, 175, 203, 231, 259, 287, 315, 343, 370, 397, 424,
    451, 478, 505, 532, 559, 586, 613, 640, 8, 36, 64, 92, 120, 148, 176, 204, 232,
    260, 288, 316, 344, 371, 398, 425, 452, 479, 506, 533, 560, 587, 614, 641, 9,
    37, 65, 93, 121, 149, 177, 205, 233, 261, 289, 317, 345, 372, 399, 426, 453, 480,
    507, 534, 561, 588, 615, 642, 10, 38, 66, 94, 122, 150, 178, 206, 234, 262, 290,
    318, 346, 373, 400, 427, 454, 481, 508, 535, 562, 589, 616, 643, 11, 39, 67, 95,
    123, 151, 179, 207, 235, 263, 291, 319, 347, 374, 401, 428, 455, 482, 509, 536,
    563, 590, 617, 644, 12, 40, 68, 96, 124, 152, 180, 208, 236, 264, 292, 320, 348,
    375, 402, 429, 456, 483, 510, 537, 564, 591, 618, 645, 13, 41, 69, 97, 125, 153,
    181, 209, 237, 265, 293, 321, 349, 376, 403, 430, 457, 484, 511, 538, 565, 592,
    619, 646, 14, 42, 70, 98, 126, 154, 182, 210, 238, 266, 294, 322, 350, 377, 404,
    431, 458, 485, 512, 539, 566, 593, 620, 647, 15, 43, 71, 99, 127, 155, 183, 211,
    239, 267, 295, 323, 351, 378, 405, 432, 459, 486, 513, 540, 567, 594, 621, 648,
    16, 44, 72, 100, 128, 156, 184, 212, 240, 268, 296, 324, 352, 379, 406, 433, 460,
    487, 514, 541, 568, 595, 622, 649, 17, 45, 73, 101, 129, 157, 185, 213, 241, 269,
    297, 325, 353, 380, 407, 434, 461, 488, 515, 542, 569, 596, 623, 650, 18, 46, 74,
    102, 130, 158, 186, 214, 242, 270, 298, 326, 354, 381, 408, 435, 462, 489, 516,
    543, 570, 597, 624, 651, 19, 47, 75, 103, 131, 159, 187, 215, 243, 271, 299, 327,
    355, 382, 409, 436, 463, 490, 517, 544, 571, 598, 625, 652, 20, 48, 76, 104, 132,
    160, 188, 216, 244, 272, 300, 328, 356, 383, 410, 437, 464, 491, 518, 545, 572,
    599, 626, 653, 21, 49, 77, 105, 133, 161, 189, 217, 245, 273, 301, 329, 357, 384,
    411, 438, 465, 492, 519, 546, 573, 600, 627, 654, 22, 50, 78, 106, 134, 162, 190,
    218, 246, 274, 302, 330, 358, 385, 412, 439, 466, 493, 520, 547, 574, 601, 628,
    655, 23, 51, 79, 107, 135, 163, 191, 219, 247, 275, 303, 331, 359, 386, 413, 440,
    467, 494, 521, 548, 575, 602, 629, 656, 24, 52, 80, 108, 136, 164, 192, 220, 248,
    276, 304, 332, 360, 387, 414, 441, 468, 495, 522, 549, 576, 603, 630, 657, 25, 53,
    81, 109, 137, 165, 193, 221, 249, 277, 305, 333, 361, 388, 415, 442, 469, 496, 523,
    550, 577, 604, 631, 658, 26, 54, 82, 110, 138, 166, 194, 222, 250, 278, 306, 334,
    362, 389, 416, 443, 470, 497, 524, 551, 578, 605, 632, 659, 27, 55, 83, 111, 139,
    167, 195, 223, 251, 279, 307, 335};



void dstar_scramble(BOOL * in, BOOL * out, uint32 length, uint32 * scramble_count)
{
    if ( out == NULL || in == NULL || scramble_count == NULL) {
        output("Null inOut pointer\n");
        return;
    }

    uint32 i = 0;

    for ( i = 0 ; i < length ; i++ ) {
        out[i] = in[i] ^ SCRAMBLER_TABLE_BITS[(*scramble_count)++];

        if (*scramble_count >= SCRAMBLER_TABLE_BITS_LENGTH)
            *scramble_count = 0U;
    }
}


void dstar_interleave(const BOOL * in, BOOL * out, unsigned int length)
{
    if ( in == NULL || out == NULL ) {
        output(ANSI_RED "Null in or out in interleave\n" ANSI_WHITE);
        return;
    }

//    if ( length != FEC_SECTION_LENGTH_BITS ) {
//        output(ANSI_RED "length not correct interleave\n" ANSI_WHITE);
//        return;
//    }

    memset(out, 0, FEC_SECTION_LENGTH_BITS * sizeof(BOOL));
    uint32 i = 0;
    for ( i = 0 ; i < FEC_SECTION_LENGTH_BITS ; i++ ) {
        if (in[i]) {
            unsigned int newi = INTERLEAVE_TABLE[i];

            if ( newi >= FEC_SECTION_LENGTH_BITS ) {
                output(ANSI_RED "Out of range index interleave\n" ANSI_WHITE);
            }

            out[newi] = TRUE;
        }
    }
}

void dstar_deinterleave(const BOOL * in, BOOL * out, unsigned int length)
{

    memset(out, 0, FEC_SECTION_LENGTH_BITS * sizeof(BOOL));

    uint32 k = 0;
    uint32 i = 0;
    for ( i = 0 ; i < length ; i++ ) {
        if ( k >=  FEC_SECTION_LENGTH_BITS ) {
            output(ANSI_RED "k greater than FEC_SECTION_LENGTH_BITS - deinterleave\n" ANSI_WHITE);
        }

        if (in[i])
            out[k] = TRUE;

        k += 24U;
        if (k >= 672U)
            k -= 671U;
        else if (k >= 660U)
            k -= 647U;
    }
}


/****** FEC DECOE ************/

enum FEC_STATE {
    S0,
    S1,
    S2,
    S3
};


void dstar_FECacs(DSTAR_FEC fec, unsigned int n, int* metric)
{
    //output("ACS\n");
    int tempMetric[4];

    // Pres. state = S0, Prev. state = S0 & S2
    int m1 = metric[0] + fec->metric[0];
    int m2 = metric[4] + fec->metric[2];
    if (m1 < m2) {
        fec->mem0[n] = FALSE;
        tempMetric[0]    = m1;
    } else {
        fec->mem0[n] = TRUE;
        tempMetric[0]    = m2;
    }

    // Pres. state = S1, Prev. state = S0 & S2
    m1 = metric[1] + fec->metric[0];
    m2 = metric[5] + fec->metric[2];
    if (m1 < m2) {
        fec->mem1[n] = FALSE;
        tempMetric[1]    = m1;
    } else {
        fec->mem1[n] = TRUE;
        tempMetric[1]    = m2;
    }

    // Pres. state = S2, Prev. state = S2 & S3
    m1 = metric[2] + fec->metric[1];
    m2 = metric[6] + fec->metric[3];
    if (m1 < m2) {
        fec->mem2[n] = FALSE;
        tempMetric[2]    = m1;
    } else {
        fec->mem2[n] = TRUE;
        tempMetric[2]    = m2;
    }

    // Pres. state = S3, Prev. state = S1 & S3
    m1 = metric[3] + fec->metric[1];
    m2 = metric[7] + fec->metric[3];
    if (m1 < m2) {
        fec->mem3[n] = FALSE;
        tempMetric[3]    = m1;
    } else {
        fec->mem3[n] = TRUE;
        tempMetric[3]    = m2;
    }
    uint32 i = 0;
    for ( i = 0 ; i < 4 ; i++)
        fec->metric[i] = tempMetric[i];
}


void dstar_FECtraceBack(DSTAR_FEC fec, BOOL * out, unsigned int * length)
{
    //output("traceBack\n");
    // Start from the S0, t=31
    enum FEC_STATE state = S0;

    *length = 0U;
    int i = 0;
    for ( i = 329 ; i >= 0 ; i--, (*length)++) {
        switch (state) {
            case S0: // if state = S0
                //output("i = %d\n");
                if (fec->mem0[i])
                    state = S2;             // lower path
                else
                    state = S0;             // upper path
                out[i] = FALSE;
                break;

            case S1: // if state = S1
                if (fec->mem1[i])
                    state = S2;             // lower path
                else
                    state = S0;             // upper path
                out[i] = TRUE;
                break;

            case S2: // if state = S2
                if (fec->mem2[i])
                    state = S3;             // lower path
                else
                    state = S1;             // upper path
                out[i] = FALSE;
                break;

            case S3: // if state = S3
                if (fec->mem3[i])
                    state = S3;             // lower path
                else
                    state = S1;             // upper path
                out[i] = TRUE;
                break;
        }
    }
}


void dstar_FECviterbiDecode(DSTAR_FEC fec, unsigned int n, int * data)
{
    //output("ViterbiDecode\n");
    int metric[8];

    metric[0] = (data[1] ^ 0) + (data[0] ^ 0);
    metric[1] = (data[1] ^ 1) + (data[0] ^ 1);
    metric[2] = (data[1] ^ 1) + (data[0] ^ 0);
    metric[3] = (data[1] ^ 0) + (data[0] ^ 1);
    metric[4] = (data[1] ^ 1) + (data[0] ^ 1);
    metric[5] = (data[1] ^ 0) + (data[0] ^ 0);
    metric[6] = (data[1] ^ 0) + (data[0] ^ 1);
    metric[7] = (data[1] ^ 1) + (data[0] ^ 0);

    dstar_FECacs(fec, n, metric);
}


BOOL dstar_FECdecode(DSTAR_FEC fec,const BOOL * in, BOOL * out, unsigned int inLen, unsigned int * outLen)
{
    if ( in == NULL || out == NULL ) {
        output(ANSI_RED "NULL in or out in FECDecode\n" ANSI_WHITE);
        return FALSE;
    }

    uint32 i = 0;
    for ( i = 0U; i < 4U; i++)
        fec->metric[i] = 0;

    unsigned int n = 0U;
    for ( i = 0U; i < 660U; i += 2U, n++) {
        int data[2];

        if (in[i + 0U])
            data[1] = 1;
        else
            data[1] = 0;

        if (in[i + 1U])
            data[0] = 1;
        else
            data[0] = 0;

        dstar_FECviterbiDecode(fec, n, data);
    }

    dstar_FECtraceBack(fec, out, outLen);

    // Swap endian-ness
    for ( i = 0U; i < 330U; i += 8U) {
        BOOL temp;
        temp = out[i + 0U]; out[i + 0U] = out[i + 7U]; out[i + 7U] = temp;
        temp = out[i + 1U]; out[i + 1U] = out[i + 6U]; out[i + 6U] = temp;
        temp = out[i + 2U]; out[i + 2U] = out[i + 5U]; out[i + 5U] = temp;
        temp = out[i + 3U]; out[i + 3U] = out[i + 4U]; out[i + 4U] = temp;
    }

    return TRUE;
}


void dstar_FECencode(const BOOL * in, BOOL * out, unsigned int inLen, unsigned int * outLen)
{

    *outLen = 0U;
    int d1 = 0;
    int d2 = 0;
    uint32 i = 0;
    int32 j = 0;
    for ( i = 0U; i < 42U; i++) {
        for ( j = 7; j >= 0; j--) {
            int d = in[i * 8U + j] ? 1 : 0;

            int g0 = (d + d2) % 2;
            int g1 = (d + d1 + d2) % 2;

            d2 = d1;
            d1 = d;

            out[(*outLen)++] = g1 == 1;
            out[(*outLen)++] = g0 == 1;
        }
    }
}

void dstar_FECTest(void)
{
    unsigned char bytes[660/8] = {0};
    BOOL test[330] = {0};
    BOOL encoded[330*2] = {0};
    BOOL interleaved[330*2] = {0};
    BOOL deinterleaved[330*2] = {0};
    BOOL scrambled[330*2] = {0};
    BOOL descrambled[330*2] = {0};
    BOOL decoded[330] = {0};
    uint32 i = 0;
    for ( i = 0 ; i < 327- 1 ; i += 2 ) {
        test[i] = TRUE;
        test[i+1] = FALSE;
    }

    gmsk_bitsToBytes(test, bytes, 330);
    thumbDV_dump("TEST FEC IN:", bytes, 330/8); memset(bytes,0, 660/8 * sizeof(unsigned char));

    uint32 outLen = 0;
    dstar_FECencode(test, encoded, 327, &outLen);
    output("Encode outLen = %d\n", outLen);
    gmsk_bitsToBytes(encoded, bytes, outLen);
    thumbDV_dump("TEST FEC ENCODE", bytes, outLen/8);memset(bytes,0, 660/8 * sizeof(unsigned char));

    dstar_interleave(encoded, interleaved, outLen);
    gmsk_bitsToBytes(interleaved, bytes, outLen);
        thumbDV_dump("TEST INTERLEAVE", bytes, outLen/8);memset(bytes,0, 660/8 * sizeof(unsigned char));

    uint32 count = 0;
    dstar_scramble(interleaved, scrambled, outLen, &count);
    gmsk_bitsToBytes(scrambled, bytes, outLen);
    thumbDV_dump("TEST SCRAMBLE", bytes, outLen/8);memset(bytes,0, 660/8 * sizeof(unsigned char));

    count = 0;
    dstar_scramble(scrambled, descrambled, outLen, &count);
    gmsk_bitsToBytes(descrambled, bytes, outLen);
    thumbDV_dump("TEST DE-SCRAMBLE", bytes, outLen/8);memset(bytes,0, 660/8 * sizeof(unsigned char));

    dstar_deinterleave(descrambled, deinterleaved, outLen);
    gmsk_bitsToBytes(deinterleaved, bytes, outLen);
    thumbDV_dump("TEST DE-INTELEAVE", bytes, outLen/8);memset(bytes,0, 660/8 * sizeof(unsigned char));


    dstar_fec fec;
    memset(&fec, 0, sizeof(dstar_fec));
    dstar_FECdecode(&fec, deinterleaved, decoded, outLen, &outLen);
    output("Decode outLen = %d\n", outLen);
    gmsk_bitsToBytes(decoded, bytes, outLen);
    thumbDV_dump("TEST FEC Decode", bytes, outLen/8);memset(bytes,0, 660/8 * sizeof(unsigned char));


    output("True ^ True = %d\n", TRUE ^ TRUE);
    output("True ^ False = %d\n", TRUE ^ FALSE);
    output("False ^ True = %d\n", FALSE ^ TRUE);
    output("False ^ False = %d\n", FALSE ^ FALSE);



}
