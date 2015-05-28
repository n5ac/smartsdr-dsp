///*!   \file thumbdv.h
// *    \brief Functions required to communicate and decode packets from ThumbDV
// *
// *    \copyright  Copyright 2012-2014 FlexRadio Systems.  All Rights Reserved.
// *                Unauthorized use, duplication or distribution of this software is
// *                strictly prohibited by law.
// *
// *    \date 26-MAY-2015
// *    \author     Ed Gonzalez
// *
// *
// */

/* *****************************************************************************
 *
 *  Copyright (C) 2014 FlexRadio Systems.
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


#ifndef THUMBDV_THUMBDV_
#define THUMBDV_THUMBDV_

void thumbDV_init(const char * serial_device_name, int * serial_fd);
int thumbDV_openSerial(const char * tty_name);
int thumbDV_processSerial(int serial_fd);

int thumbDV_encode(int serial_fd, short * speech_in, unsigned char * packet_out, uint8 num_of_samples );
int thumbDV_decode(int serial_fd, unsigned char * packet_in, short * speech_out, uint8 bytes_in_packet);

#endif /* THUMBDV_THUMBDV_ */
