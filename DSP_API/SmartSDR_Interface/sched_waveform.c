///*!   \file sched_waveform.c
// *    \brief Schedule Wavefrom Streams
// *
// *    \copyright  Copyright 2012-2014 FlexRadio Systems.  All Rights Reserved.
// *                Unauthorized use, duplication or distribution of this software is
// *                strictly prohibited by law.
// *
// *    \date 29-AUG-2014
// *    \author 	Ed Gonzalez
// *    \mangler 	Graham / KE9H
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

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>		// for memset
#include <unistd.h>

#include "common.h"
#include "datatypes.h"
#include "hal_buffer.h"
#include "sched_waveform.h"
#include "vita_output.h"

//static Queue sched_fft_queue;
static pthread_rwlock_t _list_lock;
static BufferDescriptor _root;

static pthread_t _waveform_thread;
static BOOL _waveform_thread_abort = FALSE;

static sem_t sched_waveform_sem;

static void _dsp_convertBufEndian(BufferDescriptor buf_desc)
{
	int i;

	if(buf_desc->sample_size != 8)
	{
		//TODO: horrendous error here
		return;
	}

	for(i = 0; i < buf_desc->num_samples*2; i++)
		((int32*)buf_desc->buf_ptr)[i] = htonl(((int32*)buf_desc->buf_ptr)[i]);
}

static BufferDescriptor _WaveformList_UnlinkHead(void)
{
	BufferDescriptor buf_desc = NULL;
	pthread_rwlock_wrlock(&_list_lock);

	if (_root == NULL || _root->next == NULL)
	{
		output("Attempt to unlink from a NULL head");
		pthread_rwlock_unlock(&_list_lock);
		return NULL;
	}

	if(_root->next != _root)
		buf_desc = _root->next;

	if(buf_desc != NULL)
	{
		// make sure buffer exists and is actually linked
		if(!buf_desc || !buf_desc->prev || !buf_desc->next)
		{
			output( "Invalid buffer descriptor");
			buf_desc = NULL;
		}
		else
		{
			buf_desc->next->prev = buf_desc->prev;
			buf_desc->prev->next = buf_desc->next;
			buf_desc->next = NULL;
			buf_desc->prev = NULL;
		}
	}

	pthread_rwlock_unlock(&_list_lock);
	return buf_desc;
}

static void _WaveformList_LinkTail(BufferDescriptor buf_desc)
{
	pthread_rwlock_wrlock(&_list_lock);
	buf_desc->next = _root;
	buf_desc->prev = _root->prev;
	_root->prev->next = buf_desc;
	_root->prev = buf_desc;
	pthread_rwlock_unlock(&_list_lock);
}

void sched_waveform_Schedule(BufferDescriptor buf_desc)
{
	_WaveformList_LinkTail(buf_desc);
	sem_post(&sched_waveform_sem);
}

void sched_waveform_signal()
{
	sem_post(&sched_waveform_sem);
}


/* *********************************************************************************************
 * *********************************************************************************************
 * *********************                                                 ***********************
 * *********************  LOCATION OF MODULATOR / DEMODULATOR INTERFACE  ***********************
 * *********************                                                 ***********************
 * *********************************************************************************************
 * ****************************************************************************************** */

#include <stdio.h>
#include "freedv_api.h"
#include "circular_buffer.h"
#include "resampler.h"

#define PACKET_SAMPLES  128

#define SCALE_RX_IN  	 8000.0 	// Multiplier   // Was 16000 GGH Jan 30, 2015
#define SCALE_RX_OUT     8000.0		// Divisor
#define SCALE_TX_IN     24000.0 	// Multiplier   // Was 16000 GGH Jan 30, 2015
#define SCALE_TX_OUT    24000.0 	// Divisor

#define FILTER_TAPS	48
#define DECIMATION_FACTOR 	3

/* These are offsets for the input buffers to decimator */
#define MEM_24		FILTER_TAPS					   /* Memory required in 24kHz buffer */
#define MEM_8		FILTER_TAPS/DECIMATION_FACTOR   /* Memory required in 8kHz buffer */



static struct freedv *_freedvS;         // Initialize Coder structure
static struct my_callback_state  _my_cb_state;
#define MAX_RX_STRING_LENGTH 40
static char _rx_string[MAX_RX_STRING_LENGTH + 5];

static BOOL _end_of_transmission = FALSE;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Circular Buffer Declarations

float RX1_buff[(PACKET_SAMPLES * 12)+1];		// RX1 Packet Input Buffer
short RX2_buff[(PACKET_SAMPLES * 12)+1];		// RX2 Vocoder input buffer
short RX3_buff[(PACKET_SAMPLES * 12)+1];		// RX3 Vocoder output buffer
float RX4_buff[(PACKET_SAMPLES * 12)+1];		// RX4 Packet output Buffer

float TX1_buff[(PACKET_SAMPLES * 12) +1];		// TX1 Packet Input Buffer
short TX2_buff[(PACKET_SAMPLES * 12)+1];		// TX2 Vocoder input buffer
short TX3_buff[(PACKET_SAMPLES * 12)+1];		// TX3 Vocoder output buffer
float TX4_buff[(PACKET_SAMPLES * 12)+1];		// TX4 Packet output Buffer

circular_float_buffer rx1_cb;
Circular_Float_Buffer RX1_cb = &rx1_cb;
circular_short_buffer rx2_cb;
Circular_Short_Buffer RX2_cb = &rx2_cb;
circular_short_buffer rx3_cb;
Circular_Short_Buffer RX3_cb = &rx3_cb;
circular_float_buffer rx4_cb;
Circular_Float_Buffer RX4_cb = &rx4_cb;

circular_float_buffer tx1_cb;
Circular_Float_Buffer TX1_cb = &tx1_cb;
circular_short_buffer tx2_cb;
Circular_Short_Buffer TX2_cb = &tx2_cb;
circular_short_buffer tx3_cb;
Circular_Short_Buffer TX3_cb = &tx3_cb;
circular_float_buffer tx4_cb;
Circular_Float_Buffer TX4_cb = &tx4_cb;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Callbacks for embedded ASCII stream, transmit and receive

void my_put_next_rx_char(void *callback_state, char c)
{
    char new_char[2];
    if ( (uint32) c < 32 || (uint32) c > 126 ) {
    	/* Treat all control chars as spaces */
    	//output(ANSI_YELLOW "Non-valid RX_STRING char. ASCII code = %d\n", (uint32) c);
    	new_char[0] = (char) 0x7F;
    } else if ( c == ' ' ) {
    	/* Encode spaces differently */
    	new_char[0] = (char) 0x7F;
    } else {
    	new_char[0] = c;
    }

    new_char[1] = 0;

    strncat(_rx_string, new_char, MAX_RX_STRING_LENGTH+4);
    if (strlen(_rx_string) > MAX_RX_STRING_LENGTH)
    {
        // lop off first character
        strcpy(_rx_string, _rx_string+1);
    }
    //output(ANSI_MAGENTA "new string = '%s'\n",_rx_string);

    char* api_cmd = safe_malloc(80);
    sprintf(api_cmd, "waveform status slice=%d string=\"%s\"",0,_rx_string);
    tc_sendSmartSDRcommand(api_cmd,FALSE,NULL);
    safe_free(api_cmd);
}

struct my_callback_state
{
    char  tx_str[80];
    char *ptx_str;
};

char my_get_next_tx_char(void *callback_state)
{
    struct my_callback_state* pstate = (struct my_callback_state*)callback_state;
    char  c = *pstate->ptx_str++;

    if (*pstate->ptx_str == 0)
    {
        pstate->ptx_str = pstate->tx_str;
    }

    return c;
}

void freedv_set_string(uint32 slice, char* string)
{
    strcpy(_my_cb_state.tx_str, string);
    _my_cb_state.ptx_str = _my_cb_state.tx_str;
    output(ANSI_MAGENTA "new TX string is '%s'\n",string);
}

void sched_waveform_setEndOfTX(BOOL end_of_transmission)
{
    _end_of_transmission = TRUE;
}



static void* _sched_waveform_thread(void* param)
{
    int 	nin, nout;

    int		i;			// for loop counter
    float	fsample;	// a float sample
//    float   Sig2Noise;	// Signal to noise ratio

    // Flags ...
    int		initial_tx = 1; 		// Flags for TX circular buffer, clear if starting transmit
    int		initial_rx = 1;			// Flags for RX circular buffer, clear if starting receive

	// VOCODER I/O BUFFERS
    short	speech_in[FREEDV_NSAMPLES];
    short 	speech_out[FREEDV_NSAMPLES];
    short 	demod_in[FREEDV_NSAMPLES];
    short 	mod_out[FREEDV_NSAMPLES];

    // RX RESAMPLER I/O BUFFERS
    float 	float_in_8k[PACKET_SAMPLES + FILTER_TAPS];
    float 	float_out_8k[PACKET_SAMPLES];

    float 	float_in_24k[PACKET_SAMPLES * DECIMATION_FACTOR + FILTER_TAPS];
    float 	float_out_24k[PACKET_SAMPLES * DECIMATION_FACTOR ];

    // TX RESAMPLER I/O BUFFERS
    float 	tx_float_in_8k[PACKET_SAMPLES + FILTER_TAPS];
    float 	tx_float_out_8k[PACKET_SAMPLES];

    float 	tx_float_in_24k[PACKET_SAMPLES * DECIMATION_FACTOR + FILTER_TAPS];
    float 	tx_float_out_24k[PACKET_SAMPLES * DECIMATION_FACTOR ];

    BOOL inhibit_tx = FALSE;
    BOOL flush_tx = FALSE;


    // =======================  Initialization Section =========================
    _freedvS = freedv_open(FREEDV_MODE_1600);	// Default system, only
    //assert(_freedvS != NULL);					// debug only

    // Initialize the Circular Buffers

	RX1_cb->size  = PACKET_SAMPLES*6 +1;		// size = no.elements in array+1
	RX1_cb->start = 0;
	RX1_cb->end	  = 0;
	RX1_cb->elems = RX1_buff;
	RX2_cb->size  = PACKET_SAMPLES*6 +1;		// size = no.elements in array+1
	RX2_cb->start = 0;
	RX2_cb->end	  = 0;
	RX2_cb->elems = RX2_buff;
	RX3_cb->size  = PACKET_SAMPLES*6 +1;		// size = no.elements in array+1
	RX3_cb->start = 0;
	RX3_cb->end	  = 0;
	RX3_cb->elems = RX3_buff;
	RX4_cb->size  = PACKET_SAMPLES*12 +1;		// size = no.elements in array+1
	RX4_cb->start = 0;
	RX4_cb->end	  = 0;
	RX4_cb->elems = RX4_buff;

	TX1_cb->size  = PACKET_SAMPLES*6 +1;		// size = no.elements in array+1
	TX1_cb->start = 0;
	TX1_cb->end	  = 0;
	TX1_cb->elems = TX1_buff;
	TX2_cb->size  = PACKET_SAMPLES*6 +1;		// size = no.elements in array+1
	TX2_cb->start = 0;
	TX2_cb->end	  = 0;
	TX2_cb->elems = TX2_buff;
	TX3_cb->size  = PACKET_SAMPLES *6 +1;		// size = no.elements in array+1
	TX3_cb->start = 0;
	TX3_cb->end	  = 0;
	TX3_cb->elems = TX3_buff;
	TX4_cb->size  = PACKET_SAMPLES *12 +1;		// size = no.elements in array+1
	TX4_cb->start = 0;
	TX4_cb->end	  = 0;
	TX4_cb->elems = TX4_buff;

	initial_tx = TRUE;
	initial_rx = TRUE;

    // initialize the rx callback
    _freedvS->freedv_put_next_rx_char = &my_put_next_rx_char;

    // Set up callback for txt msg chars
    // clear tx_string
    memset(_my_cb_state.tx_str,0,80);
    _my_cb_state.ptx_str = _my_cb_state.tx_str;
    _freedvS->callback_state = (void*)&_my_cb_state;
    _freedvS->freedv_get_next_tx_char = &my_get_next_tx_char;

    uint32 bypass_count = 0;
    BOOL bypass_demod = TRUE;

	// show that we are running
	BufferDescriptor buf_desc;

	while( !_waveform_thread_abort )
	{
		// wait for a buffer descriptor to get posted
		sem_wait(&sched_waveform_sem);

		if(!_waveform_thread_abort)
		{
			do {
				buf_desc = _WaveformList_UnlinkHead();
				// if we got signalled, but there was no new data, something's wrong
				// and we'll just wait for the next packet
				if (buf_desc == NULL)
				{
					//output( "We were signaled that there was another buffer descriptor, but there's not one here");
					break;
				}
				else
				{
					// convert the buffer to little endian
					_dsp_convertBufEndian(buf_desc);

					//output(" \"Processed\" buffer stream id = 0x%08X\n", buf_desc->stream_id);

					if( (buf_desc->stream_id & 1) == 0) { //RX BUFFER
						//	If 'initial_rx' flag, clear buffers RX1, RX2, RX3, RX4
						if(initial_rx)
						{
							RX1_cb->start = 0;	// Clear buffers RX1, RX2, RX3, RX4
							RX1_cb->end	  = 0;
							RX2_cb->start = 0;
							RX2_cb->end	  = 0;
							RX3_cb->start = 0;
							RX3_cb->end	  = 0;
							RX4_cb->start = 0;
							RX4_cb->end	  = 0;


							/* Clear filter memory */
							memset(float_in_24k, 0, MEM_24 * sizeof(float));
							memset(float_in_8k, 0, MEM_8 * sizeof(float));

							/* Requires us to set initial_rx to FALSE which we do at the end of
							 * the first loop
							 */
						}

						//	Set the transmit 'initial' flag
						initial_tx = TRUE;
						inhibit_tx = FALSE;
						flush_tx = FALSE;
						_end_of_transmission = FALSE;
						// Check for new receiver input packet & move to RX1_cb.
						// TODO - If transmit packet, discard here?


						for( i = 0 ; i < PACKET_SAMPLES ; i++)
						{
							//output("Outputting ")
							//	fsample = Get next float from packet;
							cbWriteFloat(RX1_cb, ((Complex*)buf_desc->buf_ptr)[i].real);

						}

//
						// Check for >= 384 samples in RX1_cb and spin downsampler
						//	Convert to shorts and move to RX2_cb.
						if(cfbContains(RX1_cb) >= 384)
						{
							for(i=0 ; i<384 ; i++)
							{
								float_in_24k[i + MEM_24] = cbReadFloat(RX1_cb);
							}

							fdmdv_24_to_8(float_out_8k, &float_in_24k[MEM_24], 128);

							for(i=0 ; i<128 ; i++)
							{
								cbWriteShort(RX2_cb, (short) (float_out_8k[i]*SCALE_RX_IN));

							}

						}
//
//						// Check for >= 320 samples in RX2_cb and spin vocoder
						// 	Move output to RX3_cb.
//						do {
							nin = freedv_nin(_freedvS); // TODO Is nin, nout really necessary?

							if ( csbContains(RX2_cb) >= nin )
							{
	//
								for( i=0 ; i< nin ; i++)
								{
									demod_in[i] = cbReadShort(RX2_cb);
								}

								nout = freedv_rx(_freedvS, speech_out, demod_in);



								if ( _freedvS->fdmdv_stats.sync ) {
									/* Increase count for turning bypass off */
									if ( bypass_count < 10) bypass_count++;
								} else {
									if ( bypass_count > 0 ) bypass_count--;
								}

								if ( bypass_count > 7 ) {
									//if ( bypass_demod ) output("baypass_demod transitioning to FALSE\n");

									bypass_demod = FALSE;
								}
								else if ( bypass_count < 2 ) {
									//if ( !bypass_demod ) output("baypass_demod transitioning to TRUE \n");
									bypass_demod = TRUE;
								}
								if ( bypass_demod ) {
									for ( i = 0 ; i < nin ; i++ ) {
										cbWriteShort(RX3_cb, demod_in[i]);
									}
								} else {
									for( i=0 ; i < nout ; i++)
									{
										cbWriteShort(RX3_cb, speech_out[i]);
									}
								}

								//output("%d\n", bypass_count);

							}
	//						} else {
		//						break; /* Break out of while loop */
							//}
						//} while (1);
//
						// Check for >= 128 samples in RX3_cb, convert to floats
						//	and spin the upsampler. Move output to RX4_cb.

						if(csbContains(RX3_cb) >= 128)
						{
							for( i=0 ; i<128 ; i++)
							{
								float_in_8k[i+MEM_8] = ((float)  (cbReadShort(RX3_cb) / SCALE_RX_OUT)     );
							}

							fdmdv_8_to_24(float_out_24k, &float_in_8k[MEM_8], 128);

							for( i=0 ; i<384 ; i++)
							{
								cbWriteFloat(RX4_cb, float_out_24k[i]);
							}
							//Sig2Noise = (_freedvS->fdmdv_stats.snr_est);
						}

						// Check for >= 128 samples in RX4_cb. Form packet and
						//	export.

						uint32 check_samples = PACKET_SAMPLES;

						if(initial_rx)
							check_samples = PACKET_SAMPLES * 3;

						if(cfbContains(RX4_cb) >= check_samples )
						{
							for( i=0 ; i<128 ; i++)
							{
								//output("Fetching from end buffer \n");
								// Set up the outbound packet
								fsample = cbReadFloat(RX4_cb);
								// put the fsample into the outbound packet

								((Complex*)buf_desc->buf_ptr)[i].real = fsample;
								((Complex*)buf_desc->buf_ptr)[i].imag = fsample;

							}
						} else {
							//output("RX Starved buffer out\n");

							memset( buf_desc->buf_ptr, 0, PACKET_SAMPLES * sizeof(Complex));

							if(initial_rx)
								initial_rx = FALSE;
						}

						emit_waveform_output(buf_desc);

					} else if ( (buf_desc->stream_id & 1) == 1) { //TX BUFFER
						//	If 'initial_rx' flag, clear buffers TX1, TX2, TX3, TX4
						if(initial_tx)
						{
							TX1_cb->start = 0;	// Clear buffers RX1, RX2, RX3, RX4
							TX1_cb->end	  = 0;
							TX2_cb->start = 0;
							TX2_cb->end	  = 0;
							TX3_cb->start = 0;
							TX3_cb->end	  = 0;
							TX4_cb->start = 0;
							TX4_cb->end	  = 0;


							/* Clear filter memory */

							memset(tx_float_in_24k, 0, MEM_24 * sizeof(float));
							memset(tx_float_in_8k, 0, MEM_8 * sizeof(float));

							/* Requires us to set initial_rx to FALSE which we do at the end of
							 * the first loop
							 */
						}


						initial_rx = TRUE;
						// Check for new receiver input packet & move to TX1_cb.
						// TODO - If transmit packet, discard here?

						if ( !inhibit_tx ) {
                            for( i = 0 ; i < PACKET_SAMPLES ; i++ )
                            {
                                //output("Outputting ")
                                //	fsample = Get next float from packet;
                                cbWriteFloat(TX1_cb, ((Complex*)buf_desc->buf_ptr)[i].real);

                            }

    //
                            // Check for >= 384 samples in TX1_cb and spin downsampler
                            //	Convert to shorts and move to TX2_cb.
                            if(cfbContains(TX1_cb) >= 384)
                            {
                                for(i=0 ; i<384 ; i++)
                                {
                                    tx_float_in_24k[i + MEM_24] = cbReadFloat(TX1_cb);
                                }

                                fdmdv_24_to_8(tx_float_out_8k, &tx_float_in_24k[MEM_24], 128);

                                for(i=0 ; i<128 ; i++)
                                {
                                    cbWriteShort(TX2_cb, (short) (tx_float_out_8k[i]*SCALE_TX_IN));

                                }

                            }
    //
    //						// Check for >= 320 samples in TX2_cb and spin vocoder
                            // 	Move output to TX3_cb.


                                if ( csbContains(TX2_cb) >= 320 )
                                {
                                    for( i=0 ; i< 320 ; i++)
                                    {
                                        speech_in[i] = cbReadShort(TX2_cb);
                                    }

                                    freedv_tx(_freedvS, mod_out, speech_in);

                                    for( i=0 ; i < 320 ; i++)
                                    {
                                        cbWriteShort(TX3_cb, mod_out[i]);
                                    }
                                }

                            // Check for >= 128 samples in TX3_cb, convert to floats
                            //	and spin the upsampler. Move output to TX4_cb.

                            if(csbContains(TX3_cb) >= 128)
                            {
                                for( i=0 ; i<128 ; i++)
                                {
                                    tx_float_in_8k[i+MEM_8] = ((float)  (cbReadShort(TX3_cb) / SCALE_TX_OUT));
                                }

                                fdmdv_8_to_24(tx_float_out_24k, &tx_float_in_8k[MEM_8], 128);

                                for( i=0 ; i<384 ; i++)
                                {
                                    cbWriteFloat(TX4_cb, tx_float_out_24k[i]);
                                }
                                //Sig2Noise = (_freedvS->fdmdv_stats.snr_est);
                            }
					    }
						// Check for >= 128 samples in RX4_cb. Form packet and
						//	export.

						uint32 tx_check_samples = PACKET_SAMPLES;

						if(initial_tx)
							tx_check_samples = PACKET_SAMPLES * 3;

						if ( _end_of_transmission )
						    flush_tx = TRUE;

						if ( !inhibit_tx ) {
                            if(cfbContains(TX4_cb) >= tx_check_samples )
                            {
                                for( i = 0 ; i < PACKET_SAMPLES ; i++)
                                {
                                    //output("Fetching from end buffer \n");
                                    // Set up the outbound packet
                                    fsample = cbReadFloat(TX4_cb);
                                    // put the fsample into the outbound packet
                                    ((Complex*)buf_desc->buf_ptr)[i].real = fsample;
                                    ((Complex*)buf_desc->buf_ptr)[i].imag = fsample;
                                }
                            } else {
                                //output("TX Starved buffer out\n");

                                memset( buf_desc->buf_ptr, 0, PACKET_SAMPLES * sizeof(Complex));

                                if(initial_tx)
                                    initial_tx = FALSE;
                            }

                            emit_waveform_output(buf_desc);

                            if ( flush_tx ) {
                                inhibit_tx = TRUE;

                                while ( cfbContains(TX4_cb) > 0 ) {

                                    if ( cfbContains(TX4_cb) > PACKET_SAMPLES ) {
                                        for( i = 0 ; i < PACKET_SAMPLES ; i++)
                                        {
                                            // Set up the outbound packet
                                            fsample = cbReadFloat(TX4_cb);

                                            // put the fsample into the outbound packet
                                            ((Complex*)buf_desc->buf_ptr)[i].real = fsample;
                                            ((Complex*)buf_desc->buf_ptr)[i].imag = fsample;
                                        }
                                    } else {
                                        int end_index = 0;
                                        for ( i = 0 ; i <= cfbContains(TX4_cb); i++ ) {
                                            fsample = cbReadFloat(TX4_cb);
                                            ((Complex*)buf_desc->buf_ptr)[i].real = fsample;
                                            ((Complex*)buf_desc->buf_ptr)[i].imag = fsample;
                                            end_index = i+1;
                                        }

                                        for ( i = end_index ; i < PACKET_SAMPLES ; i++ ) {
                                            ((Complex*)buf_desc->buf_ptr)[i].real = 0.0f;
                                            ((Complex*)buf_desc->buf_ptr)[i].imag = 0.0f;
                                        }

                                    }
                                    emit_waveform_output(buf_desc);

                                }
                            }

						}
					}




					hal_BufferRelease(&buf_desc);
				}
			} while(1); // Seems infinite loop but will exit once there are no longer any buffers linked in _Waveformlist
		}
	}
	_waveform_thread_abort = TRUE;
	 freedv_close(_freedvS);
	return NULL;
}

void sched_waveform_Init(void)
{
	pthread_rwlock_init(&_list_lock, NULL);

	pthread_rwlock_wrlock(&_list_lock);
	_root = (BufferDescriptor)safe_malloc(sizeof(buffer_descriptor));
	memset(_root, 0, sizeof(buffer_descriptor));
	_root->next = _root;
	_root->prev = _root;
	pthread_rwlock_unlock(&_list_lock);

	sem_init(&sched_waveform_sem, 0, 0);

	pthread_create(&_waveform_thread, NULL, &_sched_waveform_thread, NULL);

	struct sched_param fifo_param;
	fifo_param.sched_priority = 30;
	pthread_setschedparam(_waveform_thread, SCHED_FIFO, &fifo_param);
}

void sched_waveformThreadExit()
{
	_waveform_thread_abort = TRUE;
	sem_post(&sched_waveform_sem);
}
