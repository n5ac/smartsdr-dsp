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


const char* APP_NAME = "ThumbDV";            // Name of Application - must match .cfg filename

char * cfg_path = NULL;

static sem_t shutdown_sem;

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
    	sprintf(syscom,"eu-addr2line -e %s %p","./thumbdv" , array[i]);
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

int main( int argc, char * argv[])
{
	const char * console_param = "--console";
	const char * restrict_ip_param = "--ip=";
	const char * config_path_param = "--cfg_path=";
	BOOL enable_console = FALSE;
	char * restrict_ip = NULL;

	/* Semaphore will be used to signal end of execution */
	sem_init(&shutdown_sem, 0, 0);
	/* If compiled in DEBUG then seg-faults will include a stack trace */
	setup_segfault_handler();

	int i = 0;
	for ( i = 1 ; i < argc; i++ ) {
		if (strncmp(argv[i], console_param, strlen(console_param)) == 0 ) {
			/* We will run with a console for input.
			 * This is normally disabled so that the waveform can run as a
			 * service or as a subprocess.
			 */
			enable_console = TRUE;
			output(ANSI_YELLOW" WAVEFORM Version 1.0.3, Console alive.\n"ANSI_WHITE);
		} else if ( strncmp(argv[i], restrict_ip_param, strlen(restrict_ip_param)) == 0 ) {
			/* Free if param was passed in twice */
			if ( restrict_ip ) {
				safe_free( restrict_ip );
				restrict_ip = NULL;
			}
			restrict_ip = safe_malloc(strlen(argv[i])+1);
			strncpy(restrict_ip, argv[i]+strlen(restrict_ip_param), strlen(argv[i]));
			output("Restrict IP = '%s'\n", restrict_ip);
		} else if ( strncmp(argv[i], config_path_param ,strlen(config_path_param)) == 0 ) {
			/* Free if param was passed in twice */
			if ( cfg_path ) {
				safe_free( cfg_path ) ;
				cfg_path = NULL;
			}
			cfg_path = safe_malloc(strlen(argv[i])+1);
			strncpy(cfg_path, argv[i] + strlen(config_path_param), strlen(argv[i]));
			output("Config Path = '%s'\n", cfg_path);
		} else {
			output("Unknown console parameter - '%s'\n", argv[i]);
		}
	}

	if ( ! cfg_path ) {
		cfg_path = safe_malloc(strlen("./") + 1);
		strncpy(cfg_path, "./", strlen("./") + 1);
	}

    SmartSDR_API_Init(enable_console, restrict_ip);

    if ( restrict_ip ) {
    	safe_free(restrict_ip);
    }

    /* Wait to be notified of shutdown */
    sem_wait(&shutdown_sem);

    safe_free(cfg_path);

    return 0;
}



