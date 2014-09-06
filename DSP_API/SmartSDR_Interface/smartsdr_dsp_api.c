///*    \file smartsdr_dsp_api.c
// *    \brief Main SmartSDR DSP API Entry point
// *
// *    \copyright  Copyright 2011-2013 FlexRadio Systems.  All Rights Reserved.
// *                Unauthorized use, duplication or distribution of this software is
// *                strictly prohibited by law.
// *
// *    \date 31-AUG-2014
// *    \author Stephen Hicks, N5AC
// *    \author Graham Haddock, KE9H
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

#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>          // File Control functions

#include "common.h"
#include "traffic_cop.h"
#include "discovery_client.h"
#include "sched_waveform.h"


static uint32 _api_version;
static uint32 _handle;
static pthread_t _console_thread_ID;
static BOOL console_thread_abort = FALSE;
#define PROMPT "\n\033[92mWaveform -->\033[33m"
static sem_t _startup_sem, _communications_sem;

extern const char* APP_NAME;
extern const char* CFG_FILE;

void api_setVersion(uint32 version)
{
    _api_version = version;
    output(ANSI_MAGENTA "version = %d.%d.%d.%d\n",_api_version >> 24,_api_version >> 16 & 0xFF,
            _api_version >> 8 & 0xFF,_api_version & 0xFF);
}

uint32 api_getVersion(void)
{
    return _api_version;
}

void api_setHandle(uint32 handle)
{
    _handle = handle;
    output(ANSI_MAGENTA "handle = 0x%08X\n",handle);

    // let everyone know we've established communications with the radio
    sem_post(&_communications_sem);
}

uint32 api_getHandle(void)
{
    return _handle;
}

void SmartSDR_API_Shutdown(void)
{
    // stop the keepalive thread if we are done
    tc_abort();
}

void* _console_thread(void* param)
{
    cmd_banner();
    // let everybody know we're through printing
    sem_post(&_startup_sem);
    sem_wait(&_communications_sem);
    output(PROMPT);
    while (!console_thread_abort)
    {
        command();
        output(PROMPT);
    }

    SmartSDR_API_Shutdown();
    return NULL;
}

void SmartSDR_API_Init(void)
{
    sem_init(&_startup_sem,0,0);
    sem_init(&_communications_sem,0,0);

    // initialize printed output
    lock_printf_init();
    lock_malloc_init();
    vita_output_Init();
    sched_waveform_Init();

    // Start the console thread
    pthread_create(&_console_thread_ID, NULL, &_console_thread, NULL);

    // wait for the console to print out all it's stuff
    sem_wait(&_startup_sem);
    tc_Init();
    dc_Init();
}

/* *****************************************************************************
 *  uint32 register_mode(void)
 *
 *      Transmits configuration information from ConfigFile.cfg
 *      Expected file contents are ASCII text.
 */

uint32 register_mode(void)
{
    FILE* cfgStream;        // Stream ID for config file when opened
    ssize_t numRead;        // Number of bytes read by getline()

    size_t nbytes = 120;    // Default size of input buffer
    char *inputBuffer;      // The input buffer
    // ssize_t result;          // strncmp() result
    char* charptr;          // pointer returned by strstr()
    BOOL readFlag;          // file readflag  [TRUE -> continue]
    struct stat statbuf;
    char MinRadioVerString[40];


    // Check for existence of file before opening, otherwise will segfault.
    if(stat(CFG_FILE, &statbuf) == 0)
    {
        output(ANSI_WHITE"Configuration file exists.\n");
    }
    else
    {
        output(ANSI_RED"CONFIGURATION FILE DOES NOT EXIST.\n");
        usleep(1000000);
        return 999;
    }

    cfgStream = fopen(CFG_FILE, "r");
    if (ferror(cfgStream))
    {
        output(ANSI_YELLOW"Error opening file %s \n", CFG_FILE);
        return 999;
    }
    else{

    }
    /* Transfer data expecting to encounter end of input (or an error) */
    inputBuffer = (char *) malloc (nbytes + 1); // Initial buffer size

    readFlag = TRUE;
    while(readFlag)     // Look for the start of the [header]
    {
        numRead = getline(&inputBuffer, &nbytes, cfgStream);
        if (numRead == -1)
        {
            output(ANSI_YELLOW"Error reading config file %s\n", CFG_FILE);
            return 999;
        }

        // Process it here
        charptr = strstr(inputBuffer, "[header]");
        if(charptr != NULL)
        {
            output(ANSI_CYAN "FreeDV: Start of [header] found.\n");
            readFlag = FALSE;
        }
        else
        {
            if (ferror(cfgStream))
            {
                output(ANSI_YELLOW"Read error %s, reached end of file, [header] not found. \n", CFG_FILE);
                return 999;   // should return a fail    return -1;
            }
        }
    }

    readFlag = TRUE;
    while(readFlag)     // Look for the minimum version
    {
        numRead = getline(&inputBuffer, &nbytes, cfgStream);
        if (numRead == -1)
        {
            output(ANSI_YELLOW"Error reading config file %s\n", CFG_FILE);
            // TODO return here?
        }

        // Process it here
        charptr = strstr(inputBuffer, "Minimum-SmartSDR-Version:");
        if(charptr != NULL)
        {
            output(ANSI_CYAN "FreeDV: Minimum Version found.\n");
            charptr += strlen("Minimum-SmartSDR-Version:");
            strcpy(MinRadioVerString, charptr );
            readFlag = FALSE;
        }
        else
        {
            if (ferror(cfgStream))
            {
                output(ANSI_YELLOW"Read error %s, minimum version not found, reached end of file. \n", CFG_FILE);
                return 999;
            }
        }
    }

    readFlag = TRUE;
    while(readFlag)     // Find the start of the [setup] section
    {
        numRead = getline(&inputBuffer, &nbytes, cfgStream);
        if (numRead == -1)
        {
            output(ANSI_YELLOW"Error reading config file %s\n", CFG_FILE);
            return 999;
        }

        // Process it here
        charptr = strstr(inputBuffer, "[setup]");
        if(charptr != NULL)
        {
            output(ANSI_CYAN "FreeDV: Start of [setup] found.\n");
            readFlag = FALSE;
        }
        else
        {
            if (ferror(cfgStream))
            {
                output(ANSI_YELLOW"Read error %s, [setup] not found. \n", CFG_FILE);
                return 999;   // should return a fail    return -1;
            }
        }
    }

    readFlag = TRUE;
    while(readFlag)     // Export the setup configuration information
    {                   //  Stop on "[end]" or EOF

        numRead = getline(&inputBuffer, &nbytes, cfgStream);
        if (numRead == -1)
        {
            output("Error reading config file %s\n", CFG_FILE);
            return 999;
        }

        // turn trailing 'carriage returns" to nulls.
        charptr = strstr(inputBuffer, "\r");
        if(charptr != NULL)
            *charptr = 0;


        charptr = strstr(inputBuffer, "[end]");
        if(strlen(inputBuffer) == 0)
        {
            output(ANSI_CYAN "FreeDV: Blank line, not sent. \n");
        }
        else if(charptr != NULL)
        {
            output(ANSI_CYAN "FreeDV: Script end marker [end] found.\n");
            readFlag = FALSE;
        }
        else if (ferror(cfgStream))
        {
            output("End of file %s, reached. \n\n", CFG_FILE);
            readFlag = FALSE;
        }
        else
        {
            // Process it here
            output(ANSI_CYAN "FreeDV: %s \n", inputBuffer);
            tc_sendSmartSDRcommand(inputBuffer, FALSE, NULL);
        }
    }

    if (fclose(cfgStream) == EOF)
    {
        output(ANSI_YELLOW"Error closing config file %s\n", CFG_FILE);
    }
    else
    {
        output(ANSI_CYAN "FreeDV: SUCCESS, closed config file %s\n", CFG_FILE);
    }

    return SUCCESS;
}

