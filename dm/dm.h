/*
 * Copyright (c) 2015 Karastelev Nikolai <rus.nick@mail.ru>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef DM_H
#define DM_H

#define _GNU_SOURCE
#include <syslog.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <syslog.h>



#define DM_OPEN_LOCK_FILE_ERR 	-1;
#define DM_READ_LOCK_FILE_ERR 	-2;
#define DM_ACTIVE_PROCESS_ERR 	-3;
#define DM_LOCK_FILE_EXIST_ERR 	-4;
#define DM_EXCLUSIVE_LOCK_ERR 	-5;
#define DM_INITIAL_FORK_ERR 	-6;
#define DM_PROCESS_SESSION_ERR 	-7;
#define DM_WRITE_LOCK_FILE_ERR 	-8;
#define DM_SET_SIGNAL_ERR 		-9;

struct dm_handlers_t {
	void (*fatal_sig_handler)(int);// these signals mainly indicate fault conditions and should be logged.
	void (*term_handler)(int);		// TERM  - shut down immediately
	void (*hup_handler)(int);		// HUP - this could be used to force a re-read of a configuration file for example
	void (*usr1_handler)(int);		// USR1 - finish serving the current connection and then close down
};

/* Make the current process a deamon
 * 
 * lock_file_name - (input parameter) the name of lock file. Create if not exist or will be read if exist.
 * lock_file_desc - (output parameter) the lock file descriptor. 
 * 				Note: the lock file descriptor will be close by using dm_terminate function.
 * pid 			  - (output parameter) the pid of deamon process
 * 
 * Return non negative value if success. If error see ERR constant and stderr message
 * 
 */
int dm_initiation(const char *const lock_file_name, int *const lock_file_desc, pid_t *const pid);

/* Set deamon signal hadlers
 * 
 * handlers - (input parameter) signal hadlers struct
 * 
 * Return non negative value if success. If error see ERR constant and stderr message
 */
int dm_set_signal_handlers(struct dm_handlers_t *handlers);

/* Termintate deamon
 * 
 * lock_file_name - (input parameter) the name of lock file.
 * lock_file_desc - (output parameter) the lock file descriptor. 
 * 
 * Close and delete lock file. Close logging.
 */
void dm_terminate(const char *const lock_file_name, int *lock_file_desc);

/* Send SIGUSR1 signal, catching by usr1_handler() function
 * 
 * lock_file_name - (input parameter) the name of lock file.
 * 
 * Return non negative value if success.
 */
int dm_stop(const char *const lock_file_name);

/* Send SIGHUP signal, catching by hup_handler() function
 * 
 * lock_file_name - (input parameter) the name of lock file.
 * 
 * Return non negative value if success.
 */
int dm_restart(const char *const lock_file_name);

#endif /* DM_H */
