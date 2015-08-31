///*!   \file thumbdv.c
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

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include "common.h"
#include "datatypes.h"
#include "hal_buffer.h"

#include "vita_output.h"
#include "thumbDV.h"
#include "sched_waveform.h"


#define DV3000_TTY              "/dev/ttyAMA0"
#define DV3000_VERSION          "2014-04-23"

#define AMBE3000_HEADER_LEN     4U
#define AMBE3000_START_BYTE     0x61U

#define AMBE3000_SPEECHD_HEADER_LEN 3U

#define AMBE3000_CTRL_PKT_TYPE      0x00
#define AMBE3000_SPEECH_PKT_TYPE    0x02
#define AMBE3000_CHAN_PKT_TYPE      0x01

#define BUFFER_LENGTH           400U
#define THUMBDV_MAX_PACKET_LEN  2048U

static pthread_t _read_thread;
BOOL _readThreadAbort = FALSE;

static uint32 _buffering_target = 4;

static pthread_rwlock_t _encoded_list_lock;
static BufferDescriptor _encoded_root;
static BOOL _encoded_buffering = TRUE;
static uint32 _encoded_count = 0;

static pthread_rwlock_t _decoded_list_lock;
static BufferDescriptor _decoded_root;
static BOOL _decoded_buffering = TRUE;
static uint32 _decoded_count = 0;

static void * _thumbDV_readThread( void * param );

static BufferDescriptor _thumbDVEncodedList_UnlinkHead( void ) {
    BufferDescriptor buf_desc = NULL;
    pthread_rwlock_wrlock( &_encoded_list_lock );


    if ( _encoded_root == NULL || _encoded_root->next == NULL ) {
        output( "Attempt to unlink from a NULL head" );
        pthread_rwlock_unlock( &_encoded_list_lock );
        return NULL;
    }

    if ( _encoded_buffering ) {
        pthread_rwlock_unlock( &_encoded_list_lock );
        return NULL;
    }

    if ( _encoded_root->next != _encoded_root )
        buf_desc = _encoded_root->next;

    if ( buf_desc != NULL ) {
        // make sure buffer exists and is actually linked
        if ( !buf_desc || !buf_desc->prev || !buf_desc->next ) {
            output( "Invalid buffer descriptor" );
            buf_desc = NULL;
        } else {
            buf_desc->next->prev = buf_desc->prev;
            buf_desc->prev->next = buf_desc->next;
            buf_desc->next = NULL;
            buf_desc->prev = NULL;

            if ( _encoded_count > 0 ) _encoded_count--;
        }
    } else {
        if ( !_encoded_buffering ) output( "Encoded list now buffering\n" );

        _encoded_buffering = TRUE;
    }

    pthread_rwlock_unlock( &_encoded_list_lock );
    return buf_desc;
}

static void _thumbDVEncodedList_LinkTail( BufferDescriptor buf_desc ) {
    pthread_rwlock_wrlock( &_encoded_list_lock );
    buf_desc->next = _encoded_root;
    buf_desc->prev = _encoded_root->prev;
    _encoded_root->prev->next = buf_desc;
    _encoded_root->prev = buf_desc;
    _encoded_count++;

    if ( _encoded_count > _buffering_target ) {
        if ( _encoded_buffering ) output( "Encoded Buffering is now FALSE\n" );

        _encoded_buffering = FALSE;
    }

    pthread_rwlock_unlock( &_encoded_list_lock );
}

static BufferDescriptor _thumbDVDecodedList_UnlinkHead( void ) {
    BufferDescriptor buf_desc = NULL;
    pthread_rwlock_wrlock( &_decoded_list_lock );

    if ( _decoded_root == NULL || _decoded_root->next == NULL ) {
        output( "Attempt to unlink from a NULL head" );
        pthread_rwlock_unlock( &_decoded_list_lock );
        return NULL;
    }

    if ( _decoded_buffering ) {
        pthread_rwlock_unlock( &_decoded_list_lock );
        return NULL;
    }

    if ( _decoded_root->next != _decoded_root ) {
        buf_desc = _decoded_root->next;
    }

    if ( buf_desc != NULL ) {
        //output("0");
        // make sure buffer exists and is actually linked
        if ( !buf_desc || !buf_desc->prev || !buf_desc->next ) {
            output( "Invalid buffer descriptor" );
            buf_desc = NULL;
        } else {
            buf_desc->next->prev = buf_desc->prev;
            buf_desc->prev->next = buf_desc->next;
            buf_desc->next = NULL;
            buf_desc->prev = NULL;

            if ( _decoded_count > 0 ) _decoded_count--;
        }
    } else {
        if ( !_decoded_buffering )
            output( "DecodedList now Buffering \n" );

        _decoded_buffering = TRUE;
    }

    pthread_rwlock_unlock( &_decoded_list_lock );
    return buf_desc;
}

static void _thumbDVDecodedList_LinkTail( BufferDescriptor buf_desc ) {
    pthread_rwlock_wrlock( &_decoded_list_lock );
    buf_desc->next = _decoded_root;
    buf_desc->prev = _decoded_root->prev;
    _decoded_root->prev->next = buf_desc;
    _decoded_root->prev = buf_desc;

    _decoded_count++;

    if ( _decoded_count > _buffering_target ) {
        if ( _decoded_buffering ) output( "Decoded Buffering is now FALSE\n" );

        _decoded_buffering = FALSE;
    }

    pthread_rwlock_unlock( &_decoded_list_lock );
}

static void delay( unsigned int delay ) {
    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = delay * 1000UL;
    nanosleep( &tim, &tim2 );
};

void thumbDV_dump( char * text, unsigned char * data, unsigned int length ) {
    unsigned int offset = 0U;
    unsigned int i;

    output( "%s", text );
    output( "\n" );

    while ( length > 0U ) {
        unsigned int bytes = ( length > 16U ) ? 16U : length;

        output( "%04X:  ", offset );

        for ( i = 0U; i < bytes; i++ )
            output( "%02X ", data[offset + i] );

        for ( i = bytes; i < 16U; i++ )
            output( "   " );

        output( "   *" );

        for ( i = 0U; i < bytes; i++ ) {
            unsigned char c = data[offset + i];

            if ( isprint( c ) )
                output( "%c", c );
            else
                output( "." );
        }

        output( "*\n" );

        offset += 16U;

        if ( length >= 16U )
            length -= 16U;
        else
            length = 0U;
    }
}

static void thumbDV_writeSerial( int serial_fd, const unsigned char * buffer, uint32 bytes ) {
    fd_set fds;

    FD_ZERO( &fds );
    FD_SET( serial_fd, &fds );

    int32 n = 0;
    errno = 0;
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    select( serial_fd + 1, NULL, &fds, NULL, &timeout );

    if ( FD_ISSET( serial_fd, &fds ) ) {
        n = write( serial_fd , buffer, bytes );

        if ( n != bytes ) {
            output( ANSI_RED "Could not write to serial port. errno = %d\n", errno );
            return;
        }
    } else {
        output( ANSI_RED "Could not write to serial port. Timeout\n" ANSI_WHITE );
    }

}

static int _check_serial( int fd ) {
    unsigned char reset[5] = { 0x61, 0x00, 0x01, 0x00, 0x33 };
    thumbDV_writeSerial( fd, reset, 5 );

    fd_set fds;
    struct timeval timeout;

    FD_ZERO( &fds );
    FD_SET( fd, &fds );
    errno = 0;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    select( fd + 1, &fds, NULL, NULL, &timeout );

    if ( FD_ISSET( fd, &fds ) ) {
        int ret = thumbDV_processSerial( fd );

        if ( ret != 0 ) {
            output( "Could not reset serial port FD = %d \n", fd );
            return -1;
        }
    } else {
        output( "Could not reset serial port FD = %d \n", fd );
        return -1;
    }

    unsigned char get_prodID[5] = {0x61, 0x00, 0x01, 0x00, 0x30 };
    thumbDV_writeSerial( fd, get_prodID, 5 );

    FD_ZERO( &fds );
    FD_SET( fd, &fds );
    errno = 0;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    select( fd + 1, &fds, NULL, NULL, &timeout );

    if ( FD_ISSET( fd, &fds ) ) {
        int ret = thumbDV_processSerial( fd );

        if ( ret != 0 ) {
            output( "Could not get prodID serial port FD = %d \n", fd );
            return -1;
        }
    }  else {
        output( "Could not prodID serial port FD = %d \n", fd );
        return -1;
    }

    return 0 ;
}

int thumbDV_openSerial( const char * tty_name ) {
    struct termios tty;
    int fd;

    /* TODO: Sanitize tty_name */

    fd = open( tty_name, O_RDWR | O_NOCTTY | O_SYNC );

    if ( fd < 0 ) {
        output( "ThumbDV: error when opening %s, errno=%d\n", tty_name, errno );
        return fd;
    }

    if ( tcgetattr( fd, &tty ) != 0 ) {
        output( "ThumbDV: error %d from tcgetattr\n", errno );
        return -1;
    }

    cfsetospeed( &tty, B460800 );
    cfsetispeed( &tty, B460800 );

    tty.c_cflag = ( tty.c_cflag & ~CSIZE ) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;

    tty.c_oflag = 0;
    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 5;

    tty.c_iflag &= ~( IXON | IXOFF | IXANY );

    tty.c_cflag |= ( CLOCAL | CREAD );

    tty.c_cflag &= ~( PARENB | PARODD );
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if ( tcsetattr( fd, TCSANOW, &tty ) != 0 ) {
        output( "ThumbDV: error %d from tcsetattr\n", errno );
        close( fd );
        return -1;
    }

    if ( _check_serial( fd ) != 0 ) {
        output( "Could not detect ThumbDV at 460800 Baud. Trying 230400\n" );
        close( fd );
    } else {
        return fd;
    }

    fd = open( tty_name, O_RDWR | O_NOCTTY | O_SYNC );

    if ( fd < 0 ) {
        output( "ThumbDV: error when opening %s, errno=%d\n", tty_name, errno );
        return fd;
    }

    if ( tcgetattr( fd, &tty ) != 0 ) {
        output( "ThumbDV: error %d from tcgetattr\n", errno );
        return -1;
    }

    cfsetospeed( &tty, B230400 );
    cfsetispeed( &tty, B230400 );

    tty.c_cflag = ( tty.c_cflag & ~CSIZE ) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;

    tty.c_oflag = 0;
    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 5;

    tty.c_iflag &= ~( IXON | IXOFF | IXANY );

    tty.c_cflag |= ( CLOCAL | CREAD );

    tty.c_cflag &= ~( PARENB | PARODD );
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if ( tcsetattr( fd, TCSANOW, &tty ) != 0 ) {
        output( "ThumbDV: error %d from tcsetattr\n", errno );
        close( fd );
        return -1;
    }

    if ( _check_serial( fd ) != 0 ) {
        output( "Could not detect THumbDV at 230400 Baud\n" );
        close( fd );
        return -1;
    }

    return fd;
}

int thumbDV_processSerial( int serial_fd ) {
    unsigned char buffer[BUFFER_LENGTH];
    unsigned int respLen;
    unsigned int offset;
    ssize_t len;
    unsigned char packet_type;

    errno = 0;
    len = read( serial_fd, buffer, 1 );

    if ( len != 1 ) {
        output( ANSI_RED "ThumbDV: Process serial. error when reading from the serial port, len = %d, errno=%d\n" ANSI_WHITE, len, errno );
        return 1;
    }

    if ( buffer[0U] != AMBE3000_START_BYTE ) {
        output( ANSI_RED "ThumbDV: unknown byte from the DV3000, 0x%02X\n" ANSI_WHITE, buffer[0U] );
        return 1;
    }

    offset = 0U;

    while ( offset < ( AMBE3000_HEADER_LEN - 1U ) ) {
        len = read( serial_fd, buffer + 1U + offset, AMBE3000_HEADER_LEN - 1 - offset );

        if ( len == 0 )
            delay( 5UL );

        offset += len;
    }

    respLen = buffer[1U] * 256U + buffer[2U];

    offset = 0U;

    while ( offset < respLen ) {
        len = read( serial_fd, buffer + AMBE3000_HEADER_LEN + offset, respLen - offset );

        if ( len == 0 )
            delay( 5UL );

        offset += len;
    }

    respLen += AMBE3000_HEADER_LEN;

    BufferDescriptor desc = NULL;
    packet_type = buffer[3];

    //thumbDV_dump("Serial data", buffer, respLen);
    if ( packet_type == AMBE3000_CTRL_PKT_TYPE ) {
        thumbDV_dump( ANSI_YELLOW "Serial data" ANSI_WHITE, buffer, respLen );
    } else if ( packet_type == AMBE3000_CHAN_PKT_TYPE ) {
        desc = hal_BufferRequest( respLen, sizeof( unsigned char ) );
        memcpy( desc->buf_ptr, buffer, respLen );
        //thumbDV_dump(ANSI_BLUE "Coded Packet" ANSI_WHITE, buffer, respLen);
        /* Encoded data */
        _thumbDVEncodedList_LinkTail( desc );
    } else if ( packet_type == AMBE3000_SPEECH_PKT_TYPE ) {
        desc = hal_BufferRequest( respLen, sizeof( unsigned char ) );
        memcpy( desc->buf_ptr, buffer, respLen );
        //thumbDV_dump("SPEECH Packet", buffer, respLen);
        /* Speech data */
        _thumbDVDecodedList_LinkTail( desc );

    } else {
        output( ANSI_RED "Unrecognized packet type 0x%02X ", packet_type );
        return 1;

    }


    return 0;
}

int thumbDV_decode( int serial_fd, unsigned char * packet_in, short * speech_out, uint8 bytes_in_packet ) {
    uint32 i = 0;

    unsigned char full_packet[15] = {0};

    if ( packet_in != NULL && serial_fd > 0 ) {
        full_packet[0] = 0x61;
        full_packet[1] = 0x00;
        full_packet[2] = 0x0B;
        full_packet[3] = 0x01;
        full_packet[4] = 0x01;
        full_packet[5] = 0x48;
        uint32 j = 0;

        for ( i = 0, j = 8  ; i < 9 ; i++ , j-- ) {
            full_packet[i + 6] = packet_in[i];
        }

//        thumbDV_dump("Just AMBE", packet_in, 9);
//        thumbDV_dump("Encoded packet:", full_packet, 15);
        thumbDV_writeSerial( serial_fd, full_packet, 15 );
    }

    int32 samples_returned = 0;
    BufferDescriptor desc = _thumbDVDecodedList_UnlinkHead();
    uint32 samples_in_speech_packet = 0;
    uint32 length = 0;

    if ( desc != NULL ) {
        length = ( ( ( unsigned char * )desc->buf_ptr )[1] << 8 ) + ( ( unsigned char * )desc->buf_ptr )[2];;

        if ( length != 0x142 ) {
            output( ANSI_YELLOW, "WARNING LENGHT DOESN'T Match %d " ANSI_WHITE, length );
            thumbDV_dump( "MISMATHCED", ( ( unsigned char * ) desc->buf_ptr ), desc->num_samples );
        }

        samples_in_speech_packet = ( ( unsigned char * )desc->buf_ptr )[5];

        unsigned char * idx = &( ( ( unsigned char * )desc->buf_ptr )[6] );
        uint32 i = 0;

        for ( i = 0; i < samples_in_speech_packet; i++, idx += 2 ) {
            speech_out[i] = ( idx[0] << 8 ) + idx[1];
        }

        samples_returned = samples_in_speech_packet;

        if ( samples_returned != 160 ) output( "Rate Mismatch expected %d got %d\n", 160, samples_returned );

        safe_free( desc );
    } else {
        /* Do nothing for now */
    }

    return samples_returned;
}

int thumbDV_encode( int serial_fd, short * speech_in, unsigned char * packet_out, uint8 num_of_samples ) {
    unsigned char packet[THUMBDV_MAX_PACKET_LEN];
    uint16 speech_d_bytes = num_of_samples * sizeof( uint16 ); /* Should be 2 times the number of samples */

    /* Calculate length of packet NOT including the full header just the type field*/
    uint16 length = 0;
    /* Includes Channel Field and SpeechD Field Header */
    length += AMBE3000_SPEECHD_HEADER_LEN;
    length += speech_d_bytes;

    /* Will be used to write fields into packet */
    unsigned char * idx = &packet[0];

    *( idx++ ) = AMBE3000_START_BYTE;
    /* Length split into two bytes */
    *( idx++ ) = length >> 8;
    *( idx++ ) = length & 0xFF;
    /* SPEECHD Type */
    *( idx++ ) = AMBE3000_SPEECH_PKT_TYPE;
    /* Channel0 Identifier */
    *( idx++ ) = 0x40;
    /* SPEEECHD Identifier */
    *( idx++ ) = 0x00;
    /* SPEECHD No of Samples */
    *( idx++ ) = num_of_samples;
    uint32 i = 0;
//    output("Num of Samples = 0x%X\n", num_of_samples);

#ifdef WOOT
    output( "Encode Packet Header = " );
    unsigned char * p = &packet[0];
    i = 0;

    for ( i = 0 ; i < 7 ; i++ ) {

        output( "%02X ", *p );
        p++;
    }

    output( "\n" );

#endif
    //memcpy(idx, speech_in, speech_d_bytes);
    i = 0;

    for ( i = 0 ; i < num_of_samples ; i++, idx += 2 ) {
        idx[0] = speech_in[i] >> 8;
        idx[1] = ( speech_in[i] & 0x00FF ) ;
    }

    if ( serial_fd > 0 )
        thumbDV_writeSerial( serial_fd, packet, length + AMBE3000_HEADER_LEN );

    int32 samples_returned = 0;
    BufferDescriptor desc = _thumbDVEncodedList_UnlinkHead();

    if ( desc != NULL ) {
        memcpy( packet_out, desc->buf_ptr + 6, desc->sample_size * ( desc->num_samples - 6 ) );
        samples_returned = desc->num_samples - 6;
        safe_free( desc );
        //thumbDV_dump(ANSI_BLUE "Coded Packet" ANSI_WHITE, packet_out, desc->num_samples - 6);

    } else {
        /* Do nothing for now */
    }

    return samples_returned;

}


static void _connectSerial( int * serial_fd ) {
    int i = 0 ;
    char device[256] = {0} ;

    do {
        for ( i = 0 ; i < 25 ; i++ ) {
            sprintf( device, "/dev/ttyUSB%d", i );
            *serial_fd = thumbDV_openSerial( device );

            if ( *serial_fd > 0 ) {
                /* We opened a valid port */
                break;
            }
        }

        if ( * serial_fd < 0 ) {
            output( "Could not open serial. Waiting 1 second before trying again.\n" );
            usleep( 1000 * 1000 );
        }
    } while ( *serial_fd < 0 ) ;

    unsigned char reset[5] = { 0x61, 0x00, 0x01, 0x00, 0x33 };
    thumbDV_writeSerial( *serial_fd, reset, 5 );
    /* Block until we get data from serial port after reset */
    thumbDV_processSerial( *serial_fd );
//
//    unsigned char reset_softcfg[11] = {0x61, 0x00, 0x07, 0x00, 0x34, 0x05, 0x03, 0xEB, 0xFF, 0xFF, 0xFF};
//    thumbDV_writeSerial(*serial_fd, reset_softcfg, 11);
//    thumbDV_processSerial(*serial_fd);

    unsigned char disable_parity[6] = {0x61, 0x00, 0x02, 0x00, 0x3F, 0x00};
    thumbDV_writeSerial( *serial_fd, disable_parity, 6 );
    thumbDV_processSerial( *serial_fd );

    unsigned char get_prodID[5] = {0x61, 0x00, 0x01, 0x00, 0x30 };
    unsigned char get_version[5] = {0x61, 0x00, 0x01, 0x00, 0x31};
    unsigned char read_cfg[5] = {0x61, 0x00, 0x01, 0x00, 0x37};
    unsigned char dstar_mode[17] = {0x61, 0x00, 0x0D, 0x00, 0x0A, 0x01, 0x30, 0x07, 0x63, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48};

    thumbDV_writeSerial( *serial_fd, get_prodID, 5 );
    thumbDV_writeSerial( *serial_fd, get_version, 5 );
    thumbDV_writeSerial( *serial_fd, read_cfg, 5 );
    thumbDV_writeSerial( *serial_fd, dstar_mode, 17 );

////    /* Init */
    unsigned char pkt_init[6] = { 0x61, 0x00, 0x02, 0x00, 0x0B, 0x07 };
    thumbDV_writeSerial( *serial_fd, pkt_init, 6 );

    /* PKT GAIN - set to 0dB */
    unsigned char pkt_gain[7] = { 0x61, 0x00, 0x03, 0x00, 0x4B, 0x00, 0x00 };
    thumbDV_writeSerial( *serial_fd, pkt_gain, 7 );

    /* Companding off so it uses 16bit linear */
    unsigned char pkt_compand[6] = { 0x61, 0x00, 0x02, 0x00, 0x32, 0x00 };
    thumbDV_writeSerial( *serial_fd, pkt_compand, 6 );

    unsigned char test_coded[15] =  {0x61, 0x00 , 0x0B , 0x01 , 0x01 , 0x48 , 0x5E , 0x83 , 0x12 , 0x3B , 0x98 , 0x79 , 0xDE , 0x13 , 0x90};

    thumbDV_writeSerial( *serial_fd, test_coded, 15 );

    unsigned char pkt_fmt[7] = {0x61, 0x00, 0x3, 0x00, 0x15, 0x00, 0x00};
    thumbDV_writeSerial( *serial_fd, pkt_fmt, 7 );

}


static void * _thumbDV_readThread( void * param ) {
    int topFd;
    fd_set fds;
    int ret;

    int serial_fd = *( int * )param;
    topFd = serial_fd + 1;

    output( "Serial FD = %d in thumbDV_readThread(). TopFD = %d\n", serial_fd, topFd );

    struct timeval timeout;

    while ( !_readThreadAbort ) {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        FD_ZERO( &fds );
        FD_SET( serial_fd, &fds );

        errno = 0;
        ret = select( topFd, &fds, NULL, NULL, &timeout );

        if ( ret < 0 ) {
            fprintf( stderr, "ThumbDV: error from select, errno=%d\n", errno );

        }

        if ( FD_ISSET( serial_fd, &fds ) ) {
            ret = thumbDV_processSerial( serial_fd );

            if ( ret != 0 ) {
                /* Set invalid FD in sched_waveform so we don't call write functions */
                sched_waveform_setFD( -1 );
                /* This function hangs until a new connection is made */
                _connectSerial( &serial_fd );
                /* Update the sched_waveform to new valid serial */
                sched_waveform_setFD( serial_fd );
                topFd = serial_fd + 1;
            }
        }
    }

    output( ANSI_YELLOW "thumbDV_readThread has exited\n" ANSI_WHITE );
    return 0;
}

void thumbDV_init( int * serial_fd ) {
    pthread_rwlock_init( &_encoded_list_lock, NULL );
    pthread_rwlock_init( &_decoded_list_lock, NULL );

    pthread_rwlock_wrlock( &_encoded_list_lock );
    _encoded_root = ( BufferDescriptor )safe_malloc( sizeof( buffer_descriptor ) );
    memset( _encoded_root, 0, sizeof( buffer_descriptor ) );
    _encoded_root->next = _encoded_root;
    _encoded_root->prev = _encoded_root;
    pthread_rwlock_unlock( &_encoded_list_lock );

    pthread_rwlock_wrlock( &_decoded_list_lock );
    _decoded_root = ( BufferDescriptor )safe_malloc( sizeof( buffer_descriptor ) );
    memset( _decoded_root, 0, sizeof( buffer_descriptor ) );
    _decoded_root->next = _decoded_root;
    _decoded_root->prev = _decoded_root;
    pthread_rwlock_unlock( &_decoded_list_lock );

    _connectSerial( serial_fd );

    pthread_create( &_read_thread, NULL, &_thumbDV_readThread, serial_fd );

    struct sched_param fifo_param;
    fifo_param.sched_priority = 30;
    pthread_setschedparam( _read_thread, SCHED_FIFO, &fifo_param );


}
