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
 
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

 
#include "cfg/cfg.h"
#include "net/net.h"
#include "com/com.h"
#include "dm/dm.h"
#include "proto/proto.h"

#define LM_NORMAL 	1
#define LM_TRACE 	2
 
#define LOCK_FILE_NAME_SIZE 30

char 						lock_file_name[LOCK_FILE_NAME_SIZE];
int 						lock_file_desc;
pid_t 						deamon_pid;

struct cfg_exparams_t	exparams;
struct cfg_config_t 		config;

struct listener_t			listener;
struct net_packet_t		net_packet;

int 						com_id;
struct com_opt_t 			com_opt;

void inline mlog(int mode, const char * msg, ...) {
	 
	va_list args;
	va_start(args, msg);
	 
	if (mode == LM_NORMAL) {
		vprintf(msg, args);
	}
	
// If trace message mode on	
	if((mode == LM_TRACE) && (exparams.ex_stat & EXB_TRACE)) {
		vprintf(msg, args);
	}

	va_end(args);
}

int open_com_port(struct cfg_com_section_t *com, int vmin) {

	struct com_opt_t com_opt;
	
	com_opt.baund_rate 	= com->baund_rate;
    com_opt.data_bits 	= com->data_bits;
    com_opt.parity 		= com_get_parity(com->parity);
    com_opt.stop_bits 	= com->stop_bits;
    com_opt.vmin 		= vmin;
    
    int result = com_open(com->device, &com_opt);
    
    return result;    	
} 

void print_input_net_packet(struct listener_t *listener) {
	
	struct host_addr_t s_addr;
	
	net_to_host_addr(&listener->sen_host_addr, &s_addr);

	mlog(LM_TRACE, "[i] Packet %d.%d.%d.%d:%d | [ ", 
		s_addr.a, s_addr.b,	s_addr.c, s_addr.d, s_addr.port);
		
	int i;
	
	for(i = 0; i < listener->buffer_size; i++) {
		mlog(LM_TRACE, "%02x ", *((byte*)listener->buffer + i));
	} 	
	
	mlog(LM_TRACE, "]\n");	
}
 
int listen_handler(struct listener_t *listener, int received_bytes) {
	
	if (received_bytes == 0) { return 0; }
	
	print_input_net_packet(listener);
	
	struct net_packet_t *packet = (struct net_packet_t *)listener->buffer;
	
	if(!CHECK_HEADER(packet->header)) {
		mlog(LM_TRACE, "[i] Unknown paket format, packet is skipped\n");
		return 0;
	}
	
	if (packet->ctrl & CTRL_STOP_DEAMON) {
		mlog(LM_TRACE, "[i] Stop deamon command was recieved\n");
		listener->listen = NET_CLOSED;
		return 0;	
	}

	if (packet->ctrl & CTRL_SND_COM) {
		
		int com_id = *((int*)listener->tag);
		
		int nbytes = com_send_bytes(com_id, (char*)(&packet->com_data), COM_PACKET_SIZE);
		
		if (nbytes < 0) packet->ctrl |= CTRL_ERR;
		
		if (nbytes < 0) {
			mlog(LM_TRACE, "[e] COM port sending error\n");
		} else {
			mlog(LM_TRACE, "[i] Send %d bytes to COM port\n", nbytes);
		}
	}
	
	if (packet->ctrl & CTRL_RSV_COM) {
		
		int com_id = *((int*)listener->tag);
		
		int nbytes = com_timedreceive_bytes(com_id, (char*)(&packet->com_data), 
							COM_PACKET_SIZE, config.com.receive_timeout);
		
		if (nbytes < 0) packet->ctrl |= CTRL_ERR;
		
		if (nbytes < 0) {
			mlog(LM_TRACE, "[e] COM port receiving error\n");
		} else {
			mlog(LM_TRACE, "[i] Receive %d bytes from COM port\n", nbytes);
		}
	}

	if (packet->ctrl & CTRL_RSV_ANSW) {
		
		int result = net_answer(listener, packet, NET_PACKET_SIZE);

		if (result < 0) {
			mlog(LM_TRACE, "[e] Echo answer sending error\n");
		} else {
			mlog(LM_TRACE, "[i] Echo answer was sent\n");
		}
	}

	return 0;
}
 

void dm_fatal_sig_handler(int sig) {
	// These signals mainly indicate fault conditions and should be logged
	mlog(LM_NORMAL, "[e] Caught signal fatal signal - exiting\n");
	net_close(&listener);
	dm_terminate(lock_file_name, &lock_file_desc);
	exit(EXIT_FAILURE);
}

void dm_term_handler(int sig) {
	// TERM  - shut down immediately
	mlog(LM_NORMAL, "[e] Caught TERM signal - shut down immediately\n");
	net_close(&listener);
	com_close(*((int*)listener.tag));
	dm_terminate(lock_file_name, &lock_file_desc);
	exit(EXIT_FAILURE);
}

void dm_hup_handler(int sig) {
	// HUP - this could be used to force a re-read of a configuration file for example
	//mlog("[i] Caught signal SIGHUP - restar deamon\n");
	//listener.listen = NET_CLOSED;
}

void dm_usr1_handler(int sig) {
	// USR1 - soft shutdown
	mlog(LM_NORMAL, "[i] Caught signal SIGUSR1 - soft shutdown\n");
	listener.listen = NET_CLOSED;
}

struct dm_handlers_t deamon_handlers = { 
	dm_fatal_sig_handler, 
	dm_term_handler, 
	dm_hup_handler, 
	dm_usr1_handler };


int main(int argc, char *argv[]) {
	
// Read console run parameters
	int ex_stat = cfg_read_exparams(argc, argv, &exparams);
	
	if (ex_stat & EXB_ERRPARAM) {
		exit(EXB_ERRPARAM);
	}
	
	if (ex_stat & EXB_SHINFO) {
		cfg_print_exparams_info();
		exit(EXIT_SUCCESS);
	}
	
// Get deamon lock file name
	dm_get_lfname(lock_file_name, LOCK_FILE_NAME_SIZE);
	
// If stop parameters was set
	if (ex_stat & EXB_STOP) {
		int pid = dm_stop(lock_file_name);
		mlog(LM_NORMAL, "[i] Send terminate signal to deamon process with pid %d\n", pid);
		exit(EXIT_SUCCESS);
	}	

// Load cofiguration file	
	int cfg_stat = cfg_read_config(exparams.cfg_file, &config);
	
	if (cfg_stat != CFG_SUCCESS) {	
		errno = cfg_stat;
		fprintf(stderr, "[e] Can not load config file '%s'\n", exparams.cfg_file);
		exit(EXIT_FAILURE);
	}	
	
// Become deamon
	if (ex_stat & EXB_DEAMON) {

		int result = dm_initiation(lock_file_name, &lock_file_desc, &deamon_pid);
		
		if (result < 0) {
			errno = result;
			fprintf(stderr, "[e] Deamon initiation failure, exit\n");	
			exit(EXIT_FAILURE);
		}
		
		result = dm_set_signal_handlers(&deamon_handlers);
		
		if (result < 0) {
			errno = result;
			fprintf(stderr, "[e] Deamon set signal handlers error\n");	
			dm_terminate(lock_file_name, &lock_file_desc);
			exit(EXIT_FAILURE);			
		}
		
		mlog(LM_NORMAL, "[i] COM deamon started successfully\n");
		
	}
		
// Open COM port

	mlog(LM_NORMAL, "[i] Open COM port '%s'\n", config.com.device);
	
	com_opt.baund_rate 	= config.com.baund_rate;
	com_opt.data_bits 	= config.com.data_bits;
	com_opt.parity 		= com_get_parity(config.com.parity);
	com_opt.stop_bits	= config.com.stop_bits;
	com_opt.vmin 		= sizeof(struct com_packet_t);
	
	com_id = com_open(config.com.device, &com_opt);
	
	if (com_id < 0) {
		com_close(com_id);
	
		if (ex_stat & EXB_DEAMON) {
			dm_terminate(lock_file_name, &lock_file_desc);
		}
		
		errno = com_id;
		fprintf(stderr, "[e] Open COM port '%s' error\n", config.com.device);
		
		dm_terminate(lock_file_name, &lock_file_desc);	
		exit(EXIT_FAILURE);
	}

// Start net server
	mlog(LM_NORMAL, "[i] Start COM server, listen port %d\n", config.app.port);	
	
	listener.rec_host_addr.sin_family 		= AF_INET;
	listener.rec_host_addr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	listener.rec_host_addr.sin_port 		= htons((unsigned short) config.app.port);
	
	listener.listen_callback 	= listen_handler;
	listener.listen 			= NET_LISTEN;
	listener.tag 				= &com_id;
	listener.buffer 			= &net_packet;
	listener.buffer_size 		= sizeof(net_packet);
	
	int listener_result = net_listen(&listener);
	
	if (listener_result < 0) {
		mlog(LM_NORMAL, "[e] Open connection error %d\n", listener_result);	
	}
	
	if (listener_result == NET_CLOSED) {
		mlog(LM_NORMAL, "[i] Connection is closed\n");	
	}
	
// Exit	
	com_close(com_id);
	
	if (ex_stat & EXB_DEAMON) {
		dm_terminate(lock_file_name, &lock_file_desc);
	}
	
	mlog(LM_NORMAL, "[i] Exit\n");	
}


