///*!   \file gmsk_modem.c
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

/* Demod Section */
#include "DStarDefines.h"
#include "gmsk_modem.h"

const float FILTER_COEFFS_TABLE[] = {
    /* 2400 Hz */
    -0.000153959924563F,  0.000000000000000F,  0.000167227768379F,  0.000341615513437F,
     0.000513334449696F,  0.000667493753523F,  0.000783901543032F,  0.000838293462576F,
     0.000805143268199F,  0.000661865814384F,  0.000393913058926F, -0.000000000000000F,
    -0.000503471198655F, -0.001079755887508F, -0.001671728086040F, -0.002205032425392F,
    -0.002594597675000F, -0.002754194565297F, -0.002608210441859F, -0.002104352817854F,
    -0.001225654870420F,  0.000000000000000F,  0.001494548041184F,  0.003130012785731F,
     0.004735238379172F,  0.006109242742194F,  0.007040527007323F,  0.007330850462455F,
     0.006821247169795F,  0.005417521811131F,  0.003112202160626F, -0.000000000000000F,
    -0.003715739376345F, -0.007727358782391F, -0.011638713107503F, -0.014992029537478F,
    -0.017304097563429F, -0.018108937286588F, -0.017003180218569F, -0.013689829477969F,
    -0.008015928769710F,  0.000000000000000F,  0.010154104792614F,  0.022059114281395F,
     0.035162729807337F,  0.048781621388364F,  0.062148583345584F,  0.074469032280094F,
     0.084982001723750F,  0.093020219991183F,  0.098063819576269F,  0.099782731268437F,
     0.098063819576269F,  0.093020219991183F,  0.084982001723750F,  0.074469032280094F,
     0.062148583345584F,  0.048781621388364F,  0.035162729807337F,  0.022059114281395F,
     0.010154104792614F,  0.000000000000000F, -0.008015928769710F, -0.013689829477969F,
    -0.017003180218569F, -0.018108937286588F, -0.017304097563429F, -0.014992029537478F,
    -0.011638713107503F, -0.007727358782391F, -0.003715739376345F, -0.000000000000000F,
     0.003112202160626F,  0.005417521811131F,  0.006821247169795F,  0.007330850462455F,
     0.007040527007323F,  0.006109242742194F,  0.004735238379172F,  0.003130012785731F,
     0.001494548041184F,  0.000000000000000F, -0.001225654870420F, -0.002104352817854F,
    -0.002608210441859F, -0.002754194565297F, -0.002594597675000F, -0.002205032425392F,
    -0.001671728086040F, -0.001079755887508F, -0.000503471198655F, -0.000000000000000F,
     0.000393913058926F,  0.000661865814384F,  0.000805143268199F,  0.000838293462576F,
     0.000783901543032F,  0.000667493753523F,  0.000513334449696F,  0.000341615513437F,
     0.000167227768379F,  0.000000000000000F, -0.000153959924563F};

#define FILTER_COEFFS_LENGTH 103U

#define PLLMAX  0x10000U
#define PLLINC      ( PLLMAX / DSTAR_RADIO_BIT_LENGTH)
#define INC  32U


enum DEMOD_STATE gmsk_decode(GMSK_DEMOD demod, float val)
{
    enum DEMOD_STATE state = DEMOD_UNKNOWN;

    /* FIlter process */
    float out = 0;//m_filter.process(val);

    BOOL bit = out > 0.0F;

    if (bit != demod->m_prev) {
        if (demod->m_pll < (PLLMAX / 2U) ) {
            demod->m_pll += PLLINC / INC;
        }
        else {
            demod->m_pll -= PLLINC / INC;
        }
    }

    demod->m_prev = bit;

    demod->m_pll += PLLINC;

    if (demod->m_pll >= PLLMAX) {
        if (demod->m_invert)
            state = bit ? DEMOD_TRUE : DEMOD_FALSE;
        else
            state = bit ? DEMOD_FALSE : DEMOD_TRUE;

        demod->m_pll -= PLLMAX;
    }

    return state;
}

void gmskDemod_reset(GMSK_DEMOD demod )
{
    demod->m_pll  = 0U;
    demod->m_prev = FALSE;
}

/* Mod Section */


/* Init */
