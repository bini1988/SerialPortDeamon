 /* File net.c */

#include "net.h"


void net_to_sockaddr_in(const struct host_addr_t *in_host_addr, struct sockaddr_in *out_sockaddr_in) {
	
	unsigned long dest_address = 
		(in_host_addr->a << 24) | (in_host_addr->b << 16) | (in_host_addr->c << 8) | in_host_addr->d;

	out_sockaddr_in->sin_family = AF_INET;
	out_sockaddr_in->sin_addr.s_addr = htonl(dest_address);
	out_sockaddr_in->sin_port = htons(in_host_addr->port);
}

void net_to_host_addr(const struct sockaddr_in *in_sockaddr_in, struct host_addr_t *out_host_addr) {
	
	unsigned long sen_address = ntohl(in_sockaddr_in->sin_addr.s_addr);

	out_host_addr->a = (0xFF000000 & sen_address) >> 24;
	out_host_addr->b = (0x00FF0000 & sen_address) >> 16;
	out_host_addr->c = (0x0000FF00 & sen_address) >> 8;
	out_host_addr->d = (0x000000FF & sen_address);

	out_host_addr->port = ntohs(in_sockaddr_in->sin_port);
}

int net_listen(struct listener_t *listener) {
	
	listener->handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (listener->handle < 0) {
		return NET_CREATION_ERR;	
	}
	
	if (fcntl(listener->handle, F_SETFL, O_NONBLOCK) == -1) {
		return NET_SET_NBLOCK_ERR;	
	}
	
	if (bind(listener->handle, (struct sockaddr*) &(listener->rec_host_addr), sizeof(listener->rec_host_addr)) < 0) {
		return NET_BIND_ERR;
	}
	
	socklen_t sen_host_addr_size = sizeof(listener->sen_host_addr);
	
	while (listener->listen) {
		
		int received_bytes = recvfrom(listener->handle, 
			listener->buffer, 
			listener->buffer_size,
			0,
			(struct sockaddr*) &listener->sen_host_addr,
			&sen_host_addr_size);
			
		if (received_bytes > 0) {
			listener->listen_callback(listener, received_bytes);	
		}
	}
	 
	close(listener->handle);
	
	return NET_CLOSED;
}

int net_answer(struct listener_t *listener, void *buffer, int buffer_size) {

	if (listener->handle < 0) {
		return NET_SOCKET_ERR;	
	}

	int send_bytes = sendto(listener->handle, 
		buffer, 
		buffer_size,
		0,
		(struct sockaddr *)&listener->sen_host_addr,
		sizeof(listener->sen_host_addr));

	if (send_bytes < 0) {
		return NET_ANSWER_ERR;
	}
	
	return send_bytes;
}

void net_close(struct listener_t *listener) {
	
	listener->listen = NET_CLOSED;
	
	if (listener->handle > 0) {
		close(listener->handle);
	}
}
