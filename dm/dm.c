 /* File dm.c */

#include "dm.h"


static size_t log_writer(void *cookie, char const *data, size_t leng) {
	
	syslog(LOG_INFO, "%.*s", leng, data);
	
	return leng;
}

static int noop(void) { return 0; }

static cookie_io_functions_t log_std_fns = {
	(void*) noop, (void*) log_writer, (void*) noop, (void*) noop }; 

void std_to_log(FILE **pfp) {
	
	setvbuf(*pfp = fopencookie(NULL, "w", log_std_fns), NULL, _IOLBF, 0);
}


char *dm_get_lfname(char *str, int str_size) {
	
	snprintf(str, str_size, "/var/run/%s.pid", program_invocation_short_name);
	
	return str;
}

int dm_check_lock_file(const char *const lock_file_name) {
	
	int lock_fd = open(lock_file_name, O_RDWR|O_CREAT|O_EXCL, 0644); 	// try to grab the lock file
		
	if(lock_fd == -1) { 												// Perhaps the lock file already exists. Try to open it
		
		FILE *lfp = fopen(lock_file_name,"r");

		if(lfp == 0) {
			errno = DM_OPEN_LOCK_FILE_ERR;
			fprintf(stderr, "[e] Can't open lock file %s\n", lock_file_name);
			return DM_OPEN_LOCK_FILE_ERR;
		}

		char str_buf[17];
		
		char *lfs = fgets(str_buf, 16, lfp); 							// Read PID from lock file
	
		if (lfs == 0) {
			errno = DM_READ_LOCK_FILE_ERR;
			fprintf(stderr, "[e] Can not read lock file %s\n", lock_file_name);
			fclose(lfp);
			return DM_READ_LOCK_FILE_ERR;
		}
		
		int buf_len = strlen(str_buf) -1;
			
		if(str_buf[buf_len] == '\n') {
			str_buf[buf_len] = 0;
		}
			
		unsigned long lock_pid = strtoul(str_buf, (char**)0, 10);
			
		int kill_result = kill(lock_pid, 0); 							// See if that process is running
			
		if(kill_result == 0) {
			errno = DM_ACTIVE_PROCESS_ERR;
			fprintf(stderr, 
				"[e] A lock file %s has been detected. It appears it is owned by the (active) process with PID %ld\n",
				lock_file_name, lock_pid);
			return DM_ACTIVE_PROCESS_ERR; 
		} else {
			if(errno == ESRCH) {										// Non-existent process
				errno = DM_LOCK_FILE_EXIST_ERR;
				fprintf(stderr, 
					"[e] A lock file %s has been detected. It appears it is owned by the process with PID %ld, which is now defunct. Delete the lock file and try again\n",
					lock_file_name, lock_pid);			
				return DM_LOCK_FILE_EXIST_ERR;
			} else {
				errno = DM_EXCLUSIVE_LOCK_ERR;
				fprintf(stderr, "[e] Could not acquire exclusive lock on lock file\n");
				return DM_EXCLUSIVE_LOCK_ERR;
			}
		}
		
	} else {
		return lock_fd;
	}
}

int dm_set_exclusive_lock(int lock_fd) {

	struct flock exclusive_lock;
	
	exclusive_lock.l_type = F_WRLCK; 									// exclusive write lock
	exclusive_lock.l_whence = SEEK_SET; 								// use start and len
	exclusive_lock.l_len = exclusive_lock.l_start = 0; 					// whole file
	exclusive_lock.l_pid = 0; 
	
	int lock_result = fcntl(lock_fd, F_SETLK, &exclusive_lock);
	
	return lock_result;
}

int dm_write_pid(int lock_fd, int pid) {
	
	if(ftruncate(lock_fd, 0) < 0) { 									// truncate just in case file already existed 
		return -1;
	}

	char pid_str[7];

	sprintf(pid_str, "%d\n", (int)getpid());
	
	write(lock_fd, pid_str, strlen(pid_str));
	
	return lock_fd;
	
}

pid_t dm_read_pid(const char *const lock_file_name) {
	
	int lock_fd = open(lock_file_name, O_RDONLY);
	
	if (lock_fd < 0) {
		errno = DM_OPEN_LOCK_FILE_ERR;
		fprintf(stderr, "[e] Lock file '%s' not found. May be the server is not running?\n", lock_file_name);
		close(lock_fd);
		return DM_OPEN_LOCK_FILE_ERR;	
	}
	
 	#define CH_BUF_SIZE 16
 	
	char ch_buf[CH_BUF_SIZE];
	
	int str_len = read(lock_fd, ch_buf, CH_BUF_SIZE);

	ch_buf[str_len] = 0;
	
	pid_t pid = atoi(ch_buf);
	
	close(lock_fd);
	
	return pid;
}

int dm_initiation(const char *const lock_file_name, int *const lock_file_desc, pid_t *const pid) {
	
	chdir("/");
	
	int lock_fd = dm_check_lock_file(lock_file_name);
	
	if (lock_fd < 0) {
		return lock_fd;
	}

	int lock_result = dm_set_exclusive_lock(lock_fd);

	if(lock_result < 0) {
		errno = DM_EXCLUSIVE_LOCK_ERR;
		fprintf(stderr, "[e] Can't get exclusive lock file\n");
		close(lock_fd);
		return DM_EXCLUSIVE_LOCK_ERR;
	}

	int cur_pid = fork();

	switch(cur_pid) {
		case 0: 														// we are the child process
			break;
		case -1: 														// error - bail out (fork failing is very bad)
			errno = DM_INITIAL_FORK_ERR;
			fprintf(stderr, "[e] Initial fork failed\n");		
			return DM_INITIAL_FORK_ERR;
			break;
		default: 														// we are the parent, so exit
		  exit(0);
		  break;
	}

	if(setsid() < 0) { 													// make the process a session and process group leader
		errno = DM_PROCESS_SESSION_ERR;
		fprintf(stderr, "[e] Unable to make the process a session and process group leader\n");
		return DM_PROCESS_SESSION_ERR;
	}
	
	*pid = (int)getpid();
	
	lock_fd = dm_write_pid(lock_fd, *pid);
	
	if(lock_fd < 0) {
		errno = DM_WRITE_LOCK_FILE_ERR;
		fprintf(stderr, "[e] Unable to log PID to lock file\n");
		return DM_WRITE_LOCK_FILE_ERR;
	}

	int num_files = sysconf(_SC_OPEN_MAX);
	int i = 0;
	
	for(i = num_files - 1;i >= 0;--i) { 								// close all open files except lock
		if(i != lock_fd) {
			close(i);
		}
	}
	
	umask(0);
	// stdin/out/err to /dev/null
	int stdio_fd = open("/dev/null", O_RDWR); 	// fd 0 = stdin	
	
	// log stdout/stderr by syslog
	stdout = fopen("/dev/stdout", "w"); 	// fd 1 = stdout
	stderr = fopen("/dev/stderr", "w"); 	// fd 2 = stderr
	
	openlog(program_invocation_short_name, LOG_PID|LOG_CONS|LOG_NDELAY|LOG_NOWAIT, LOG_DAEMON);
	
	std_to_log(&stdout);
	std_to_log(&stderr);
	
	
	//int std_out = dup(stdio_fd); // fd 1 = stdout
	//int std_err = dup(stdio_fd); // fd 2 = stderr

	setpgrp();

	return 0;
}

int dm_set_signal_handlers(struct dm_handlers_t *handlers) {

	signal(SIGUSR2, SIG_IGN);	
	signal(SIGPIPE, SIG_IGN);
	signal(SIGALRM, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGURG, SIG_IGN);
	signal(SIGXCPU, SIG_IGN);
	signal(SIGXFSZ, SIG_IGN);
	signal(SIGVTALRM, SIG_IGN);
	signal(SIGPROF, SIG_IGN);
	signal(SIGIO, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);

	// these signals mainly indicate fault conditions and should be logged.
	 
	signal(SIGQUIT, handlers->fatal_sig_handler);
	signal(SIGILL,  handlers->fatal_sig_handler);
	signal(SIGTRAP, handlers->fatal_sig_handler);
	signal(SIGABRT, handlers->fatal_sig_handler);
	signal(SIGIOT,  handlers->fatal_sig_handler);
	signal(SIGBUS,  handlers->fatal_sig_handler);
	
#ifdef SIGEMT // this is not defined under Linux
	signal(SIGEMT, handlers->fatal_sig_handler);
#endif

	signal(SIGFPE, 	handlers->fatal_sig_handler);
	signal(SIGSEGV, handlers->fatal_sig_handler);
	//signal(SIGSTKFLT, handlers->fatal_sig_handler);
	signal(SIGCONT, handlers->fatal_sig_handler);
	signal(SIGPWR, 	handlers->fatal_sig_handler);
	signal(SIGSYS, 	handlers->fatal_sig_handler);
	
	// these handlers are important for control of the daemon process

	// TERM  - shut down immediately
	struct sigaction sigterm_sa;
	
	sigterm_sa.sa_handler = handlers->term_handler;
	if (sigemptyset(&sigterm_sa.sa_mask) < 0) return DM_SET_SIGNAL_ERR;
	
	sigterm_sa.sa_flags = 0;
	if (sigaction(SIGTERM, &sigterm_sa, NULL) < 0) return DM_SET_SIGNAL_ERR;
		
	// USR1 - finish serving the current connection and then close down
	struct sigaction sigusr_1sa;
		
	sigusr_1sa.sa_handler = handlers->usr1_handler;
	if (sigemptyset(&sigusr_1sa.sa_mask) < 0) return DM_SET_SIGNAL_ERR;
	
	sigusr_1sa.sa_flags = 0;
	if (sigaction(SIGUSR1, &sigusr_1sa, NULL) < 0) return DM_SET_SIGNAL_ERR;
	
	// HUP - this could be used to force a re-read of a configuration file for example
	
	struct sigaction sighup_sa;
	
	sighup_sa.sa_handler = handlers->hup_handler;
	if (sigemptyset(&sighup_sa.sa_mask) < 0) return DM_SET_SIGNAL_ERR;
	
	sighup_sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sighup_sa, NULL) < 0) return DM_SET_SIGNAL_ERR;
	
	return 0;
}

int dm_stop(const char *const lock_file_name) {
	
	pid_t pid = dm_read_pid(lock_file_name);
	
	if (pid >= 0) {
		kill(pid, SIGUSR1);
	}
	
	return pid;
}

int dm_restart(const char *const lock_file_name) {
	
	pid_t pid = dm_read_pid(lock_file_name);
	
	if (pid >= 0) {
		kill(pid, SIGHUP);
	}
	
	return pid;
}

void dm_terminate(const char *const lock_file_name, int *lock_file_desc) {
	
	if(*lock_file_desc != -1) {
		close(*lock_file_desc);
		unlink(lock_file_name);
		*lock_file_desc = -1;
	}
	
	closelog();
}
