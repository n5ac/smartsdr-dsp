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

static uint32 _buffering_target = 10;

static pthread_rwlock_t _encoded_list_lock;
static BufferDescriptor _encoded_root;
static BOOL _encoded_buffering = TRUE;
static uint32 _encoded_count = 0;

static pthread_rwlock_t _decoded_list_lock;
static BufferDescriptor _decoded_root;
static BOOL _decoded_buffering = TRUE;
static uint32 _decoded_count = 0;

static BufferDescriptor _thumbDVEncodedList_UnlinkHead(void)
{
    BufferDescriptor buf_desc = NULL;
    pthread_rwlock_wrlock(&_encoded_list_lock);


    if (_encoded_root == NULL || _encoded_root->next == NULL)
    {
        output("Attempt to unlink from a NULL head");
        pthread_rwlock_unlock(&_encoded_list_lock);
        return NULL;
    }

    if ( _encoded_buffering ) {
        pthread_rwlock_unlock(&_encoded_list_lock);
        return NULL;
    }

    if(_encoded_root->next != _encoded_root)
        buf_desc = _encoded_root->next;

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
            if ( _encoded_count > 0 ) _encoded_count--;
        }
    } else {
        if ( !_encoded_buffering ) output("Encoded list now buffering\n");
        _encoded_buffering = TRUE;
    }

    pthread_rwlock_unlock(&_encoded_list_lock);
    return buf_desc;
}

static void _thumbDVEncodedList_LinkTail(BufferDescriptor buf_desc)
{
    pthread_rwlock_wrlock(&_encoded_list_lock);
    buf_desc->next = _encoded_root;
    buf_desc->prev = _encoded_root->prev;
    _encoded_root->prev->next = buf_desc;
    _encoded_root->prev = buf_desc;
    _encoded_count++;

    if ( _encoded_count > _buffering_target ) {
        if ( _encoded_buffering ) output("Encoded Buffering is now FALSE\n");
        _encoded_buffering = FALSE;
    }

    pthread_rwlock_unlock(&_encoded_list_lock);
}

static BufferDescriptor _thumbDVDecodedList_UnlinkHead(void)
{
    BufferDescriptor buf_desc = NULL;
    pthread_rwlock_wrlock(&_decoded_list_lock);

    if (_decoded_root == NULL || _decoded_root->next == NULL)
    {
        output("Attempt to unlink from a NULL head");
        pthread_rwlock_unlock(&_decoded_list_lock);
        return NULL;
    }

    if ( _decoded_buffering ) {
        pthread_rwlock_unlock(&_decoded_list_lock);
        return NULL;
    }

    if(_decoded_root->next != _decoded_root)
        buf_desc = _decoded_root->next;

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
            if ( _decoded_count > 0 ) _decoded_count--;
        }
    } else {
        if ( !_decoded_buffering ) output("DecodedList now Buffering \n");
        _decoded_buffering = TRUE;
    }

    pthread_rwlock_unlock(&_decoded_list_lock);
    return buf_desc;
}

static void _thumbDVDecodedList_LinkTail(BufferDescriptor buf_desc)
{
    pthread_rwlock_wrlock(&_decoded_list_lock);
    buf_desc->next = _decoded_root;
    buf_desc->prev = _decoded_root->prev;
    _decoded_root->prev->next = buf_desc;
    _decoded_root->prev = buf_desc;

    _decoded_count++;
    if ( _decoded_count > _buffering_target ) {
        if ( _decoded_buffering ) output("Decoded Buffering is now FALSE\n");
        _decoded_buffering = FALSE;
    }

    pthread_rwlock_unlock(&_decoded_list_lock);
}

static void delay(unsigned int delay) {
    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = delay * 1000UL;
    nanosleep(&tim, &tim2);
};

static void dump(char *text, unsigned char *data, unsigned int length)
{
    unsigned int offset = 0U;
    unsigned int i;

    fputs(text, stdout);
    fputc('\n', stdout);

    while (length > 0U) {
        unsigned int bytes = (length > 16U) ? 16U : length;

        output( "%04X:  ", offset);

        for (i = 0U; i < bytes; i++)
            fprintf(stdout, "%02X ", data[offset + i]);

        for (i = bytes; i < 16U; i++)
            fputs("   ", stdout);

        fputs("   *", stdout);

        for (i = 0U; i < bytes; i++) {
            unsigned char c = data[offset + i];

            if (isprint(c))
                fputc(c, stdout);
            else
                fputc('.', stdout);
        }

        fputs("*\n", stdout);

        offset += 16U;

        if (length >= 16U)
            length -= 16U;
        else
            length = 0U;
    }
}

static void thumbDV_writeSerial(int serial_fd, const unsigned char * buffer, uint32 bytes)
{
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(serial_fd, &fds);

    int32 n = 0;
    errno = 0;
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    select(serial_fd + 1, NULL, &fds, NULL, &timeout);
    if ( FD_ISSET(serial_fd, &fds)) {
    n = write(serial_fd , buffer, bytes);
        if (n != bytes) {
            output(ANSI_RED "Could not write to serial port. errno = %d\n", errno);
            return;
        }
    } else {
        output(ANSI_RED "Could not write to serial port. Timeout\n" ANSI_WHITE);
    }

}

int thumbDV_openSerial(const char * tty_name)
{
    struct termios tty;
    int fd;

    /* TODO: Sanitize tty_name */

    fd = open(tty_name, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        output("ThumbDV: error when opening %s, errno=%d\n", tty_name, errno);
        return fd;
    }

    if (tcgetattr(fd, &tty) != 0) {
        output( "ThumbDV: error %d from tcgetattr\n", errno);
        return -1;
    }

    cfsetospeed(&tty, B230400);
    cfsetispeed(&tty, B230400);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;

    tty.c_oflag = 0;
    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 5;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    tty.c_cflag |= (CLOCAL | CREAD);

    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        output("ThumbDV: error %d from tcsetattr\n", errno);
        return -1;
    }

    output("opened %s - fd = %d\n", tty_name, fd);
    return fd;
}

int thumbDV_processSerial(int serial_fd)
{
    unsigned char buffer[BUFFER_LENGTH];
    unsigned int respLen;
    unsigned int offset;
    ssize_t len;
    unsigned char packet_type;

    errno = 0;
    len = read(serial_fd, buffer, 1);
    if (len != 1) {
        output(ANSI_RED "ThumbDV: error when reading from the serial port, len = %d, errno=%d\n" ANSI_WHITE, len, errno);
        return 0;
    }

    if (buffer[0U] != AMBE3000_START_BYTE) {
        output(ANSI_RED "ThumbDV: unknown byte from the DV3000, 0x%02X\n" ANSI_WHITE, buffer[0U]);
        return 1;
    }

    offset = 0U;
    while (offset < (AMBE3000_HEADER_LEN - 1U)) {
        len = read(serial_fd, buffer + 1U + offset, AMBE3000_HEADER_LEN - 1 - offset);

        if (len == 0)
            delay(5UL);

        offset += len;
    }

    respLen = buffer[1U] * 256U + buffer[2U];

    offset = 0U;
    while (offset < respLen) {
        len = read(serial_fd, buffer + AMBE3000_HEADER_LEN + offset, respLen - offset);

        if (len == 0)
            delay(5UL);

        offset += len;
    }

    respLen += AMBE3000_HEADER_LEN;

    BufferDescriptor desc = NULL;
    packet_type = buffer[3];
    //dump("Serial data", buffer, respLen);
    if ( packet_type == AMBE3000_CTRL_PKT_TYPE ) {
        dump("Serial data", buffer, respLen);
    } else if ( packet_type == AMBE3000_CHAN_PKT_TYPE ) {
        desc = hal_BufferRequest(respLen, sizeof(unsigned char) );
        memcpy(desc->buf_ptr, buffer, respLen);
        dump("Coded Packet", buffer, respLen);
        /* Encoded data */
        _thumbDVEncodedList_LinkTail(desc);
    } else if ( packet_type == AMBE3000_SPEECH_PKT_TYPE ) {
        output("s");
        desc = hal_BufferRequest(respLen, sizeof(unsigned char));
        memcpy(desc->buf_ptr, buffer, respLen);
        dump("SPEECH Packet", buffer, respLen);
        /* Speech data */
        _thumbDVDecodedList_LinkTail(desc);

    } else {
        output(ANSI_RED "Unrecognized packet type 0x%02X ", packet_type);
        safe_free(desc);
    }


    return 0;
}

int thumbDV_decode(int serial_fd, unsigned char * packet_in, short * speech_out, uint8 bytes_in_packet)
{
//
//    unsigned char * idx = &packet_in[0];
//
//    if ( *idx != AMBE3000_START_BYTE ) {
//        output(ANSI_RED "packet_in does not have valid start byte\n" ANSI_WHITE);
//        return -1;
//    }
//    idx++;
//
//    uint16 length = ( *idx << 8 ) + ( *(idx+1) );
//    //output("Packet length decode is 0x%02X", length);
//
//    if ( length != (bytes_in_packet - AMBE3000_HEADER_LEN)) {
//        output("Mismatched length %d expected %d\n", length, bytes_in_packet - AMBE3000_HEADER_LEN );
//    }
//
//    idx += 2;
//
//    if ( *idx != AMBE3000_CHAN_PKT_TYPE ) {
//        output(ANSI_RED "Invalid packet type for decode 0x%02X\n", *idx);
//        return -1;
//    }
//
//    idx++;
//
//    thumbDV_writeSerial(serial_fd, packet_in, bytes_in_packet);

    BufferDescriptor desc2  = _thumbDVEncodedList_UnlinkHead();

    if ( desc2 != NULL ) {
        thumbDV_writeSerial(serial_fd, desc2->buf_ptr, desc2->num_samples * desc2->sample_size);
    }

    int32 samples_returned = 0;
    BufferDescriptor desc = NULL;//_thumbDVDecodedList_UnlinkHead();
    uint32 samples_in_speech_packet = 0;

    if ( desc != NULL ) {
        samples_in_speech_packet = ((unsigned char * )desc->buf_ptr)[5];

       memcpy(speech_out, desc->buf_ptr + AMBE3000_HEADER_LEN + AMBE3000_SPEECHD_HEADER_LEN - 1, sizeof(uint16) * samples_in_speech_packet);
       samples_returned = samples_in_speech_packet;
       if ( samples_returned != 160 ) output("Rate Mismatch expected %d got %d\n", 160, samples_returned);
       safe_free(desc);
    } else {
        /* Do nothing for now */
    }

    return samples_returned;
}

int thumbDV_encode(int serial_fd, short * speech_in, unsigned char * packet_out, uint8 num_of_samples )
{
    unsigned char packet[THUMBDV_MAX_PACKET_LEN];
    uint16 speech_d_bytes = num_of_samples * sizeof(uint16);  /* Should be 2 times the number of samples */

    /* Calculate length of packet NOT including the full header just the type field*/
    uint16 length = 1;
    /* Includes Channel Field and SpeechD Field Header */
    length += AMBE3000_SPEECHD_HEADER_LEN;
    length += speech_d_bytes;

    /* Will be used to write fields into packet */
    unsigned char * idx = &packet[0];

    *(idx++) = AMBE3000_START_BYTE;
    /* Length split into two bytes */
    *(idx++) = length >> 8;
    *(idx++) = length & 0xFF;
    /* SPEECHD Type */
    *(idx++) = AMBE3000_SPEECH_PKT_TYPE;
    /* Channel0 Identifier */
    *(idx++) = 0x40;
    /* SPEEECHD Identifier */
    *(idx++) = 0x00;
    /* SPEECHD No of Samples */
    *(idx++) = num_of_samples;

    //output("Num of Samples = 0x%X\n", num_of_samples);

    *idx = '\0';
#ifdef WOOT
    output("Encode Packet Header = ");
    unsigned char * p = &packet[0];
    uint32 i = 0;
    for ( i = 0 ; i < 7 ; i++ ){

        output("%02X ",*p);
        p++;
    }
    output("\n");

#endif
    memcpy(idx, speech_in, speech_d_bytes);

    /* +3 needed for first 3 fields of header */
    thumbDV_writeSerial(serial_fd, packet, length + 3);

    int32 samples_returned = 0;
    BufferDescriptor desc = NULL ;//_thumbDVEncodedList_UnlinkHead();

    if ( desc != NULL ) {
        memcpy(packet_out, desc->buf_ptr, desc->sample_size * desc->num_samples);
        samples_returned = desc->num_samples;
        safe_free(desc);
    } else {
        /* Do nothing for now */
    }

    return samples_returned;

}

static void* _thumbDV_readThread(void* param)
{
    int topFd;
    fd_set fds;
    int ret;

    int serial_fd = *(int*)param;
    topFd = serial_fd + 1;

    output("Serial FD = %d in thumbDV_readThread(). TopFD = %d\n", serial_fd, topFd);

    struct timeval timeout;

    while ( !_readThreadAbort ) {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&fds);
        FD_SET(serial_fd, &fds);

        errno = 0;
        ret = select(topFd, &fds, NULL, NULL, &timeout);
        if (ret < 0) {
            fprintf(stderr, "ThumbDV: error from select, errno=%d\n", errno);
        }

        if (FD_ISSET(serial_fd, &fds)) {
            ret = thumbDV_processSerial(serial_fd);
        }
    }

    output(ANSI_YELLOW "thumbDV_readThread has exited\n" ANSI_WHITE);
    return 0;
}

void thumbDV_init(const char * serial_device_name, int * serial_fd)
{
    pthread_rwlock_init(&_encoded_list_lock, NULL);
    pthread_rwlock_init(&_decoded_list_lock, NULL);

    pthread_rwlock_wrlock(&_encoded_list_lock);
    _encoded_root = (BufferDescriptor)safe_malloc(sizeof(buffer_descriptor));
    memset(_encoded_root, 0, sizeof(buffer_descriptor));
    _encoded_root->next = _encoded_root;
    _encoded_root->prev = _encoded_root;
    pthread_rwlock_unlock(&_encoded_list_lock);

    pthread_rwlock_wrlock(&_decoded_list_lock);
    _decoded_root = (BufferDescriptor)safe_malloc(sizeof(buffer_descriptor));
    memset(_decoded_root, 0, sizeof(buffer_descriptor));
    _decoded_root->next = _decoded_root;
    _decoded_root->prev = _decoded_root;
    pthread_rwlock_unlock(&_decoded_list_lock);



    *serial_fd = thumbDV_openSerial("/dev/ttyUSB0");

    unsigned char reset[5] = { 0x61, 0x00, 0x01, 0x00, 0x33 };
    thumbDV_writeSerial(*serial_fd, reset, 5);
    /* Block until we get data from serial port after reset */
    thumbDV_processSerial(*serial_fd);

    unsigned char reset_softcfg[11] = {0x61, 0x00, 0x07, 0x00, 0x34, 0x05, 0x03, 0xEB, 0xFF, 0xFF, 0xFF};
    thumbDV_writeSerial(*serial_fd, reset_softcfg, 11);
    thumbDV_processSerial(*serial_fd);

    unsigned char disable_parity[6] = {0x61, 0x00, 0x02, 0x00, 0x3F, 0x00};
    thumbDV_writeSerial(*serial_fd, disable_parity, 6);
    thumbDV_processSerial(*serial_fd);

    pthread_create(&_read_thread, NULL, &_thumbDV_readThread, serial_fd);

    struct sched_param fifo_param;
    fifo_param.sched_priority = 30;
    pthread_setschedparam(_read_thread, SCHED_FIFO, &fifo_param);


    unsigned char get_prodID[5] = {0x61, 0x00, 0x01, 0x00, 0x30 };
    unsigned char get_version[5] = {0x61, 0x00, 0x01, 0x00, 0x31};
    unsigned char read_cfg[5] = {0x61, 0x00, 0x01, 0x00, 0x37};
    unsigned char dstar_mode[17] = {0x61, 0x00, 0x0D, 0x00, 0x0A, 0x01, 0x30, 0x07, 0x63, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48};



    thumbDV_writeSerial(*serial_fd, get_prodID, 5 );
    thumbDV_writeSerial(*serial_fd, get_version, 5);
    thumbDV_writeSerial(*serial_fd, read_cfg, 5);
    thumbDV_writeSerial(*serial_fd, dstar_mode, 17);


    unsigned char test_coded[15] =  {0x61, 0x00 ,0x0B ,0x01 ,0x01 ,0x48 ,0x5E ,0x83 ,0x12 ,0x3B ,0x98 ,0x79 ,0xDE ,0x13 ,0x90};

    thumbDV_writeSerial(*serial_fd, test_coded, 15);


}
