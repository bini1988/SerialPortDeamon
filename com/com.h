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

#ifndef COM_H
#define COM_H

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>

#define PARITY_NONE 0
#define PARITY_ODD 1
#define PARITY_EVEN 2

/* COM port options
 * 
 * baund_rate 	- set COM port speed. Avalible values: 600, 1200, 
 * 					2400, 4800. 9600, 19200, 38400, 57600, 115200
 * data_bits 	- set COM data bits. Avalible values: 5, 6, 7, 8
 * parity 		- set COM port parity. Avalible values: PARITY_ODD,
 * 					PARITY_EVEN, PARITY_NONE
 * stop_bits 	- set COM stop bits. Avalible values: 1, 2
 * vmin 		- set com_receive_bytes() returns [vmin, nbytes] when availible 
 */
struct com_opt_t {
    int baund_rate;
    int data_bits;
    int parity;
    int stop_bits;
    int vmin;
};

/* Open COM port
 * 
 *  com_name 	- COM port name
 *  opt			- COM port options struct
 * 
 * Return COM port id or negavite value if error occur
 */
int com_open(const char * com_name, struct com_opt_t * opt);

/* Send bytes to COM port
 * 
 *  com_id 		- COM port id give by calling function com_open()
 *  bytes 		- sending buffer
 *  nbytes		- sending buffer size
 * 
 * Return sending bytes
 * 
 */
int com_send_bytes(int com_id, char *bytes, int nbytes);

/* Receive bytes from COM port. Block until bytes are not received
 * 
 *  com_id 		- COM port id give by calling function com_open()
 *  bytes 		- receiving buffer
 *  nbytes		- receiving buffer size
 * 
 * Return received bytes
 * 
 */
int com_receive_bytes(int com_id, char *bytes, int nbytes);

/* Receive bytes from COM port. Block until bytes are not received or timeout is expired
 * 
 *  com_id 		- COM port id give by calling function com_open()
 *  bytes 		- receiving buffer
 *  nbytes		- receiving buffer size
 *  abs_usec_timeout - receiving timeout, usec
 * 
 * Return received bytes
 * 
 */
int com_timedreceive_bytes(int com_id, char *bytes, int nbytes, unsigned int abs_usec_timeout);

/* Close COM port
 * 
 *  com_id - COM port id give by calling function com_open()
 */
void com_close(int com_id);

/* Return parity value
 * 
 *  parity - string representaion parity value. Avalible values: 
 * 				PARITY_ODD, PARITY_EVEN, PARITY_NONE
 * 
 */
int com_get_parity(char *parity);

#endif /* COM_H */
