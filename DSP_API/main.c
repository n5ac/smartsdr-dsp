/* *****************************************************************************
 *	main.c															2014 SEP 01
 *
 *  Author: Graham / KE9H
 *  Date created: August 5, 2014
 *
 *  Wrapper program for "Embedded FreeDV" including CODEC2.
 *
 *		Derived, in part from code provided by David Rowe under LGPL in:
 *			freedv_rx.c
 *			freedv_tx.c
 *
 *		Calls and API defined by David Rowe in
 *			freedv_api.c
 *			freedv_api.h
 *
 *  Portions of this file are Copyright (C) 2014 David Rowe
 *
 * *****************************************************************************
 *
 *	Copyright (C) 2014 FlexRadio Systems.
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *****************************************************************************
 * TODO
 * 		distinguish between rx data and tx data packets?
 * 		discard rx data and tx data packet if wrong type?
 * 		get call for S/N
 *
 *
 *
 **************************************************************************** */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>			// usleep()
#include <netinet/in.h>		// ntohl()

#include <signal.h>
#include <execinfo.h>
#include <ucontext.h>

#include "smartsdr_dsp_api.h"
#include "common.h"


const char* APP_NAME = "FreeDV";            // Name of Application
const char* CFG_FILE = "FreeDV.cfg";        // Name of associated configuration file

/* This structure mirrors the one found in /usr/include/asm/ucontext.h */
typedef struct _sig_ucontext {
	unsigned long     uc_flags;
	struct ucontext   *uc_link;
	stack_t           uc_stack;
	struct sigcontext uc_mcontext;
	sigset_t          uc_sigmask;
} sig_ucontext_t;


extern char *strsignal (int __sig);
void segfault_sigaction(int signal, siginfo_t *si, void *arg)
{
    void *             array[50];
    char **            messages;
    int                size, i;
    sig_ucontext_t *   uc;


    uc = (sig_ucontext_t *)arg;

    fprintf(stderr, "\r\n\033[41;37m Caught signal %d (%s) at address 0x%08X\033[m\n", signal, strsignal(signal), (uint32)si->si_addr);

    size = backtrace(array, 50);

    messages = backtrace_symbols(array, size);

    /* skip first stack frame (points here) */
    for (i = 2; i < size && messages != NULL; ++i)
    {
    	printf("%d-",i);
    	fprintf(stderr, "TRACE: (%d) %s @ ", i, messages[i]);

    	// Get's line number of fault
    	char syscom[256];
    	sprintf(syscom,"eu-addr2line -e %s %p","./freedv" , array[i]);
    	system(syscom);
    }

    free(messages);
    exit(EXIT_FAILURE);
}

void setup_segfault_handler(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = segfault_sigaction;
    sa.sa_flags   = SA_RESTART | SA_SIGINFO;

    sigaction(SIGSEGV, &sa, NULL);

    // ignore broken pipes
    signal(SIGPIPE, SIG_IGN);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 	main()

int main(void) {

	setup_segfault_handler();
    SmartSDR_API_Init();

//			} // End of Receiver CODER processing
//			else	// Transmit ...
//			{
//                //	If 'initial' flag, clear buffers TX1, TX2, TX3, TX4
//                if(initial_tx)
//                {
//                	TX1_cb->start = 0;	// Clear buffers TX1, TX2, TX3, TX4
//                	TX1_cb->end	  = 0;
//                	TX2_cb->start = 0;
//                	TX2_cb->end	  = 0;
//                	TX3_cb->start = 0;
//                	TX3_cb->end	  = 0;
//                	TX4_cb->start = 0;
//                	TX4_cb->end	  = 0;
//                }
//
//            	//	Set the receive 'initial' flag
//            	initial_rx = 1;
//
//                // Check for new transmit input packet & move to TX1_cb.
//            	// TODO - If receive packet, discard here?
//
//            	if(InputTxPacketReady)
//            	{
//            		for(i=0;i<128;i++)
//            		{
//            			//	fsample = Get next float from packet;
//            			fsample = ntohl(fsample);	// TODO issues with/without a cast ???
//            			cbWriteFloat(RX1_cb, fsample);
//            		}
//            	}
//
//
//                // Check for >= 384 samples in TX1_cb and spin downsampler
//                //	Convert to shorts and move to TX2_cb.
//            	if(cfbContains(TX1_cb) >= 384)
//            	{
//            		for(i=0 ; i<384 ; i++)
//            		{
//            			float_in[i] = cbReadFloat(TX1_cb);
//            		}
//
//            		fdmdv_24_to_8(float_out, float_in, 384);
//
//            		for(i=0 ; i<128 ; i++)
//            		{
//            			cbWriteShort(TX2_cb, (short) float_out[i]*SCALE);
//            		}
//
//            	}
//
//                // Check for >= 320 samples in TX2_cb and spin vocoder
//                // 	Move output to TX3_cb.
//            	if(csbContains(RX2_cb) >= 320)
//            	{
//            		for( i=0 ; i<320 ; i++)
//            		{
//            			speech_in[i] = cbReadShort(TX2_cb);
//            		}
//
//            		freedv_tx(_freedvS, mod_out, speech_in);
//
//            		for( i=0 ; i<320 ; i++)
//            		{
//            			cbWriteShort(RX3_cb, mod_out[i]);
//            		}
//            	}
//
//                // Check for >= 128 samples in TX3_cb, convert to float, scale
//                //	and spin the upsampler. Move output to TX4_cb.
//
//            	if(csbContains(TX3_cb) >= 128)
//            	{
//            		for( i=0 ; i<128 ; i++)
//            		{
//            			float_in[i] = ((float) cbReadShort(TX3_cb)) / SCALE;
//            		}
//
//            		fdmdv_8_to_24(float_out, float_in, 384);
//
//            		for( i=0 ; i<384 ; i++)
//            		{
//            			cbWriteFloat(TX4_cb, float_out[i]);
//            		}
//            	}
//
//                // Check for >= 128 samples in TX4_cb. Form packet and
//                //	export.
//                if(cfbContains(TX4_cb) >= 128)
//            	{
//            		for( i=0 ; i<128 ; i++)
//            		{
//            			// Set up the outbound packet
//            			fsample = htonl( cbReadFloat(RX4_cb) );
//            			// put the fsample into the outbound packet
//            		}
//            	}
//
//			} // End of Transmit CODER processing
//		} // End of CODER ON
    	usleep(5000);
//	} // End of WHILE() loop

    while(1) {
    	usleep(100000);
    }

    return 1;
}


// EoF =====-----



