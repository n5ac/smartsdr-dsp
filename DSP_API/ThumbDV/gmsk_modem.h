///*!   \file gmsk_modem.h
// *    \date 02-JUN-2015
// *    \author Ed Gonzalez KG5FBT
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

#ifndef THUMBDV_GMSK_MODEM_H_
#define THUMBDV_GMSK_MODEM_H_

enum DEMOD_STATE {
    DEMOD_FALSE,
    DEMOD_TRUE,
    DEMOD_UNKNOWN
};

/* Used to hold state information for a GMSK Demodulator object */
typedef struct _gmsk_demod
{
    uint32  m_pll;
    BOOL    m_prev;
    BOOL    m_invert;
} gmsk_demod, * GMSK_DEMOD;

typedef struct _gmsk_mod
{
    BOOL m_invert;
} gmsk_mod, * GMSK_MOD;

uint32 gmsk_encode(GMSK_MOD mod, BOOL bit, float * buffer, unsigned int length);
enum DEMOD_STATE gmsk_decode(GMSK_DEMOD demod, float val);

void gmskDemod_reset(GMSK_DEMOD demod );

#endif /* THUMBDV_GMSK_MODEM_H_ */
