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

#ifndef PROTO_H
#define PROTO_H

#include <string.h>

#define CHECK_HEADER(x) (memcmp(x, net_header, sizeof(net_header)) == 0)

static char net_header[] = { 0x43, 0x43 };

#define CTRL_STOP_DEAMON 	0x01 // Close conncetion and exit app
#define CTRL_RSV_ANSW 		0x02 // Send answer to client
#define CTRL_SND_COM		0x04 // Send com packet to COM port
#define CTRL_RSV_COM		0x08 // Wait answer from COM port
#define CTRL_ERR			0x10 // If any error


#define COM_PACKET_SIZE sizeof(struct com_packet_t)
#define NET_PACKET_SIZE sizeof(struct net_packet_t)

typedef unsigned char byte;

/* Set COM packet format
 * 
 */
struct com_packet_t {
	byte id;
	byte conrol;
	byte data0;
	byte data1;
};

/* Sen NET packet format
 * 
 * header 	- packet header
 * ctrl 	- control value
 * com_data - com packet
 * 
 */
struct net_packet_t {
	byte header[2];
	byte ctrl;
	struct com_packet_t com_data;
};

#endif /* PROTO_H */
