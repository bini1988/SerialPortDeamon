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

#ifndef CFG_H
#define CFG_H


#define _GNU_SOURCE
#include "ini.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>

#define EXB_DEAMON 		0x01
#define EXB_HASCFG 		0x02
#define EXB_TRACE 		0x04
#define EXB_STOP		0x08
#define EXB_SHINFO		0x10
#define EXB_ERRPARAM	0x20

#define CFG_SUCCESS 	0
#define CFG_OFILE_ERR 	-1
#define CFG_MALLOC_ERR 	-2
#define CFG_PARSE_ERR 	-3

struct cfg_com_section_t {
	char* device;
    int baund_rate;
    int data_bits;
    char* parity;
    int stop_bits;
    int receive_timeout;
};

struct cfg_app_section_t {
	int port;
};

struct cfg_exparams_t {
    char* cfg_file;
    int ex_stat;
};

/* If config file is not given as execution parameter */
static char cfg_file_def[] = "/etc/comsrv.conf";

/* Possible execution parameters */
static char opt_str[] = "dc:tsh";

static const struct option long_opt_list[] = {
	{ "deamon", no_argument, NULL, 'd'},
	{ "config", required_argument, NULL, 'c'},
	{ "trace", no_argument, NULL, 't'},
	{ "stop", no_argument, NULL, 's'},
	{ "help", no_argument, NULL, 'h'},
    { NULL, no_argument, NULL, 0}
};

/* Give execution parameters info */
static char opt_str_info[] = "\n"\
	" Usage: %s [-d] [-c] [-t] [-h]\n\n"\
	"\t-d | --deamon \t Run in deamon mode\n"\
	"\t-c | --config <string> \t Set config file name\n"\
	"\t-t | --trace \t Print debug messages into console\n"\
	"\t-s | --stop \t Terminate app in deamon mode\n"\
	"\t-h | --help \t This help\n"\
	"\n";

/* Global application options structure 
 * com - COM section options
 * app - Application section options
 * 
 * See configuration file: etc/comsrv.conf
 */
struct cfg_config_t {
	struct cfg_com_section_t com; 
	struct cfg_app_section_t app;
};


/* Read config file
 *
 * 	filename 	- config file path
 *  config 		- config output varible
 *  
 * Return CFG_SUCCESS if success and negative value if error.
 * Possible errors:
 * CFG_OFILE_ERR 	IO file error
 * CFG_MALLOC_ERR	Memory allocation error
 * CFG_PARSE_ERR	COnfiguration file parse error
 * 
 * The error message is write to stderr
 * 
 */
int cfg_read_config(const char* filename, struct cfg_config_t *config);

/* Read execution parameters
 * 
 *  argc 	- parameters count
 *  argv[] 	- parameters array
 *  exparams - exectuoin parametes output
 * 
 * Return ex_stat value. The meaning of each bit is given by defined constant:
 * EXB_DEAMON 	Run as deamon
 * EXB_HASCFG 	Config file is given
 * EXB_TRACE 	Log trace messages for debug
 * EXB_STOP		Stop deamon
 * EXB_SHINFO	Get execution parametesr help
 * EXB_ERRPARAM Unknowon execution parameter is given
 */
int cfg_read_exparams(int argc, char *argv[], struct cfg_exparams_t *exparams);

/* Give execution parameters info 
 */
void cfg_print_exparams_info();

#endif /* CFG_H */
