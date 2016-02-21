 /* File com.c */

#include "com.h"

static struct termios save_termios;
struct timeval com_receive_timeout;
fd_set	com_receive_fds;

inline void set_stop_bits(int stop_bits, struct termios * com_opt) {
	
	/*
	* Stop bits set CSTOPB for 2 stop bits (1 otherwise)
	*/
    
	switch(stop_bits) {
		case 1: 
			com_opt->c_cflag &= ~CSTOPB; 
			break;
		case 2: 
		default: 
			com_opt->c_cflag |= CSTOPB; 
			break;
	}
}

inline void set_parity(int parity, struct termios * com_opt) {
	
	/*
	* Set parity checking, databits bit input
	*/
	
	switch(parity) {
		case PARITY_ODD: 
			com_opt->c_cflag &= ~(CSIZE | CSTOPB);
			com_opt->c_cflag |= (PARENB | PARODD | CS7);
			com_opt->c_iflag |= (INPCK | ISTRIP);
			break;
		case PARITY_EVEN:
			com_opt->c_cflag &= ~(CSIZE | CSTOPB | PARODD);
			com_opt->c_cflag |= (PARENB | CS7);
			com_opt->c_iflag |= (INPCK | ISTRIP);
			break;
		case PARITY_NONE: default: 
			com_opt->c_cflag &= ~(CSIZE | PARENB | CSTOPB); 
			com_opt->c_cflag |= (CS8); 
			break;
	};
	
}

inline void set_data_bits(int data_bits, struct termios * com_opt) {
	
	/*
	* Set databits bit input
	*/
	
	com_opt->c_cflag &= ~(CSIZE);

	switch(data_bits) {
		case 5: 
			com_opt->c_cflag |= CS5; 
			break;
		case 6: 
			com_opt->c_cflag |= CS6; 
			break;
		case 7: 
			com_opt->c_cflag |= CS7; 
			break;
		case 8: 
		default: 
			com_opt->c_cflag |= CS8; 
			break;
	};

}

inline int set_baund_rate(int baund_rate, struct termios * com_opt) {

	/* 
	* Set communication speed
	*/
	
	speed_t baund_rate_const;

		switch(baund_rate) {
			case 600:   
				baund_rate_const = B600;  
				break;
			case 1200:  
				baund_rate_const = B1200; 
				break;
			case 2400:  
				baund_rate_const = B2400; 
				break;
			case 4800:  
				baund_rate_const = B4800; 
				break;
			case 9600:  
				baund_rate_const = B9600; 
				break;
			case 19200: 
				baund_rate_const = B19200; 
				break;
			case 38400: 
				baund_rate_const = B38400; 
				break;
			case 57600: 
				baund_rate_const = B57600; 
				break;
			case 115200:
			default:
				baund_rate_const = B115200; 
				break;
		};

		return (cfsetispeed(com_opt, baund_rate_const) >= 0) && (cfsetospeed(com_opt, baund_rate_const) >= 0);
}


int com_open(const char * com_name, struct com_opt_t * opt) {

    int com_id = open(com_name, O_RDWR | O_NOCTTY | O_NDELAY);

    if (com_id < 0) {
		perror("Failed to open COM port");
        return -1;
        
    } else {
	
		fcntl(com_id, F_SETFL, 0); 					// Set blocking mode

		struct termios com_opt;

		if (tcgetattr(com_id, &com_opt) < 0) { 		//Set com port options
			perror("Failed to set COM port options");
			return -1;	
		}
        
		save_termios = com_opt;

		/*
		 * Echo off, echo newline off, canonical mode off, 
		 * extended input processing off, signal chars off.
		 */

		com_opt.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

		/*
		 * Turn off input processing convert break to null byte
		 * no SIGINT on BREAK, no CR to NL translation,
		 * no NL to CR translation, don't mark parity errors or breaks
		 * no input parity check, don't strip high bit off,
		 * no XON/XOFF software flow control
		 */

		com_opt.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	
		set_stop_bits(opt->stop_bits, &com_opt);

		set_parity(opt->parity, &com_opt);
		
		set_data_bits(opt->data_bits, &com_opt);

		/*
		 * Turn off output processing
		 * no CR to NL translation, no NL to CR-NL translation,
		 * no NL to CR translation, no column 0 CR suppression,
		 * no Ctrl-D suppression, no fill characters, no case mapping,
		 * no local output processing
		 */
	
		com_opt.c_oflag &= ~(OPOST);

		if (!set_baund_rate(opt->baund_rate, &com_opt)) {
			perror("Failed to set COM port baund rate");
			return -1;
		}

		/*
		 * read() returns [MIN, nbytes] when availible 
		 * Inter-character timer off
		 */

		com_opt.c_cc[VMIN]  = opt->vmin;
		com_opt.c_cc[VTIME] = 0;

    	if (tcsetattr(com_id, TCSAFLUSH, &com_opt) < 0) { // Apply the configuration

			tcsetattr(com_id, TCSAFLUSH, &save_termios); // Restore original settings if fail
			
			perror("Failed to set COM port configuration");
			return -1;
		}
    }

    return com_id;
}

int com_send_bytes(int com_id, char *bytes, int nbytes) {
 
    return write(com_id, bytes, nbytes);
}

int com_receive_bytes(int com_id, char *bytes, int nbytes) {

    return read(com_id, bytes, nbytes);
}

int com_timedreceive_bytes(int com_id, char *bytes, int nbytes, unsigned int abs_usec_timeout) {
	
	FD_ZERO(&com_receive_fds);
	FD_SET(com_id, &com_receive_fds);
	
	com_receive_timeout.tv_sec = 0;
	com_receive_timeout.tv_usec = abs_usec_timeout; 
	;

	int result = select(com_id + 1, &com_receive_fds, NULL, NULL, &com_receive_timeout);
	    
	if (result < 0) {
		perror("Failed to set COM port receive timeout");
		return -1; // Error while trying setup timeout
	} else if (result == 0) {
		perror("COM port timeout expaired");
		return 0; // Recieving timeout expaired
	} else {
		return read(com_id, bytes, nbytes);
	}  

}

void com_close(int com_id) {

	if (com_id != -1) {
		tcsetattr(com_id, TCSAFLUSH, &save_termios); 					// Restore original settings
		close(com_id);
	}
}

int com_get_parity(char *parity) {
	
	if (strcmp(parity, "PARITY_ODD") == 0) {
		return PARITY_ODD;
	} else if (strcmp(parity, "PARITY_EVEN") == 0) {
		return PARITY_EVEN;
	} else if (strcmp(parity, "PARITY_NONE") == 0) {
		return PARITY_NONE;
	} else {
		return PARITY_NONE;
	}	
}
