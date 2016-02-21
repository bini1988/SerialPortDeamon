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

#ifndef NET_H
#define NET_H

#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define MAX_UDP_PACKET_SIZE 512

#ifndef PRINT_LOG
#define PRINT_LOG 1
#endif

#define NET_CREATION_ERR 	-1
#define NET_BIND_ERR 		-2
#define NET_RECEIVE_ERR 	-3
#define NET_SOCKET_ERR 		-4
#define NET_ANSWER_ERR 		-5
#define NET_SET_NBLOCK_ERR	-6
#define NET_CLOSED			 0
#define NET_LISTEN			 1

struct listener_t;

typedef int (*listen_handler_t)(struct listener_t *listener, int received_bytes);

/* Struct to represent ip:port address
 */
struct host_addr_t {
	unsigned int a;
	unsigned int b;
	unsigned int c;
	unsigned int d;
	unsigned short port;
};


/* Listener struct
 * 
 * handle 			- socket handle
 * rec_host_addr 	- receiver (server) addres struct
 * sen_host_addr 	- sender (client) addres struct
 * listen_callback 	- listen callback fuction. Calling if buffer is full
 * buffer 			- receive buffer
 * buffer_size 		- receive buffer size
 * tag 				- void pointer for any data
 * listen 			- bool varible. Listen port until listen is true
 */
struct listener_t {
	int handle;
	struct sockaddr_in rec_host_addr;
	struct sockaddr_in sen_host_addr;
	listen_handler_t listen_callback;
	void *buffer;
	int buffer_size;
	void *tag;
	volatile sig_atomic_t listen; 
};

/* Transform host_addr struct to sockaddr_in
 */
void net_to_sockaddr_in(const struct host_addr_t *in_host_addr, struct sockaddr_in *out_sockaddr_in);

/* Transform sockaddr_in strunt to host_addr
 */
void net_to_host_addr(const struct sockaddr_in *in_sockaddr_in, struct host_addr_t *out_host_addr);

/* Create and open socket linstener
 * 
 * listener - listener options
 * 
 * Return non negative value if no error and NET_CLOSED if socket closed.
 * 
 */
int net_listen(struct listener_t *listener);

/* Send answer to last client
 * 
 * listener 	- listener options
 * buffer 		- send buffer
 * buffer_size 	- send buffer size 
 * 
 * Return sending bytes
 * 
 */
int net_answer(struct listener_t *listener, void *buffer, int buffer_size);

/* Close listeen socket
 */
void net_close(struct listener_t *listener);

#endif /* NET_H */
