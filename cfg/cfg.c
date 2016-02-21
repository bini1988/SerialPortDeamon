 /* File cfg.c */

#include "cfg.h"


static int cfg_handler(void *user, const char *section, 
			const char *name, const char *value) {
	
	struct cfg_config_t *config = (struct cfg_config_t *)user;
	
	#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	
	// APP section
	if (MATCH("app", "port")) {
		config->app.port = atoi(value);
	}
	
	// COM section
	if (MATCH("com", "device")) {
		config->com.device = strdup(value);
	} else if (MATCH("com", "baund_rate")) {
		config->com.baund_rate = atoi(value);
	} else if (MATCH("com", "data_bits")) {
		config->com.data_bits = atoi(value);
	} else if (MATCH("com", "parity")) {
		config->com.parity = strdup(value);
	} else if (MATCH("com", "stop_bits")) {
		config->com.stop_bits = atoi(value);
	} if (MATCH("com", "receive_timeout")) {
		config->com.receive_timeout = atoi(value);
	}
	
}

int cfg_read_config(const char* filename, struct cfg_config_t *config) {

	int result = ini_parse(filename, cfg_handler, config);
	
	if (result == CFG_OFILE_ERR) {
		errno = CFG_OFILE_ERR;
		fprintf(stderr, "[e] Config file '%s' open error\n", filename);
	}
	
	if (result == CFG_MALLOC_ERR) {
		errno = CFG_MALLOC_ERR;
		fprintf(stderr, "[e] Can not load config file '%s', memory allocation error\n", filename);
	}

	if (result > 0) {
		errno = CFG_PARSE_ERR;
		fprintf(stderr, "[e] Can not load config file '%s', parseing error in line %d\n", filename, result);
	}
	
	return result;
}

int cfg_read_exparams(int argc, char *argv[], struct cfg_exparams_t *exparams) {
	
    int long_index = 0;
	int result = 0;
	
	opterr = 0;
	
	exparams->cfg_file 	= cfg_file_def;
	
    int opt = getopt_long(argc, argv, opt_str, long_opt_list, &long_index);

    while(opt != -1) {
		
		if (opt == 'd') {
			result |= EXB_DEAMON;
		} else if (opt == 'c') {
			exparams->cfg_file = strdup(optarg);
			result |= EXB_HASCFG;
		} else if (opt == 't') {
			result |= EXB_TRACE;
		} else if (opt == 's') {
			result |= EXB_STOP;				
		} else if (opt == 'h') {
			result |= EXB_SHINFO;
		} else if (opt == '?') {
			errno = EXB_ERRPARAM;
			fprintf(stderr, "[e] Unknown execution parameters, run %s -h for getting help info\n", program_invocation_short_name);
			result |= EXB_ERRPARAM;
		}		

		opt = getopt_long(argc, argv, opt_str, long_opt_list, &long_index);
    }	
    
	exparams->ex_stat = result;
	
    return result;
}

void cfg_print_exparams_info() {
	
    printf(opt_str_info, program_invocation_short_name);
}

