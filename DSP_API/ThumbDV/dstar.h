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

typedef struct _dstar_fec
{
    BOOL mem0[330];
    BOOL mem1[330];
    BOOL mem2[330];
    BOOL mem3[330];
    int metric[4];
} dstar_fec, * DSTAR_FEC;
void dstar_FECTest(void);


void dstar_scramble(BOOL * in, BOOL * out, uint32 length, uint32 * scramble_count);
void dstar_interleave(const BOOL * in, BOOL * out, unsigned int length);
void dstar_deinterleave(const BOOL * in, BOOL * out, unsigned int length);
BOOL dstar_FECdecode(DSTAR_FEC fec, const BOOL * in, BOOL * out, unsigned int inLen, unsigned int * outLen);
#endif /* THUMBDV_DSTAR_H_ */
