/* peakfinder, 2005 Hal Finkel */
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#ifdef HAVE_LIBREADLINE
#  if defined(HAVE_READLINE_READLINE_H)
#    include <readline/readline.h>
#  elif defined(HAVE_READLINE_H)
#    include <readline.h>
#  else /* !defined(HAVE_READLINE_H) */
extern char *readline();
#  endif /* !defined(HAVE_READLINE_H) */
char *cmdline = NULL;
#else /* !defined(HAVE_READLINE_READLINE_H) */
/* no readline */
#endif /* HAVE_LIBREADLINE */

#ifdef HAVE_READLINE_HISTORY
#  if defined(HAVE_READLINE_HISTORY_H)
#    include <readline/history.h>
#  elif defined(HAVE_HISTORY_H)
#    include <history.h>
#  else /* !defined(HAVE_HISTORY_H) */
extern void add_history();
extern int write_history();
extern int read_history();
extern void using_history();
#  endif /* defined(HAVE_READLINE_HISTORY_H) */
/* no history */
#endif /* HAVE_READLINE_HISTORY */

#include "commands.h"
#include "common.h"

/**
 * The generated parsing function
 * @return Parsing status code
 */
int yyparse();

#define HISTORY_FILE ".peakfind_history" /** The name of the command-history file */
#define MAXCMDLINE 1024 /** If readline is not available, the maximum number of characters per line */

int should_quit; /** Should the main loop terminate */
struct pf_cal_fit current_cal; /** The current calibration */
struct pf_data current_data; /** The current data set */
int has_data; /** Valid data-set flag */

/**
 * Parse and then execute a command
 * @param[in] command The command to parse and execute
 * @return Status code
 */
int 
pf_parse_command(char* command) 
{
	int command_length;

	if (!command) {
		return 1;
	}

	command_length = strlen(command);
	if (command_length == 0) {
		return 1;
	}

	pf_set_command_parse_buffer(command);
	if (yyparse() != 0) {
		printf("command failed, please try again...\n");
	}
	else {
		pf_execute_command();
	}

	pf_free_parse_data();

#ifdef HAVE_READLINE_HISTORY
	add_history(command);
#endif

	return 1;
}

/**
 * Execute the previously-parsed command
 */
void 
pf_execute_command() 
{
	enum pf_command next_command;

	next_command = pf_get_parsed_command();
	if (next_command == command_quit) {
		should_quit = 1;
	}
	else if (next_command == command_load) {
		if (pf_loaddata(pf_get_parsed_file(), &current_data)) {
			has_data = 1;
		}
	}
	else if (next_command == command_print_data) {
		if (has_data) {
			pf_printdata(&current_data, pf_get_parsed_file(), pf_get_parsed_norm());
		}
		else {
			printf("no data to print\n");
		}
	}
	else if (next_command == command_print_peaks) {
		if (has_data) {
			pf_printpeaks(&current_data, pf_get_parsed_file(), pf_get_parsed_sort());
		}
		else {
			printf("no data to print\n");
		}
	}
	else if (next_command == command_print_peak) {
		if (has_data) {
			pf_printpeak(&current_data, &current_cal, pf_get_parsed_peak(), pf_get_parsed_file());
		}
		else {
			printf("no data to print\n");
		}
	}
	else if (next_command == command_print_calibration) {
		if (current_cal.valid) {
			pf_printcal(&current_data, &current_cal, pf_get_parsed_file());
		}
		else {
			printf("no calibration to print\n");
		}
	}
	else if (next_command == command_plot_data) {
		if (has_data) {
			struct pf_plot_params pp;

			pf_get_parsed_plot_params(&pp);
			pf_plotdata(&current_data, &pp, &current_cal);
		}
		else {
			printf("no data to plot\n");
		}
	}
	else if (next_command == command_plot_peak) {
		if (has_data) {
			struct pf_plot_params pp;
		
			pf_get_parsed_plot_params(&pp);
			pf_plotpeak(&current_data, &pp, &current_cal);
		}
		else {
			printf("no data to plot\n");
		}
	}
	else if (next_command == command_calibrate) {
		if (has_data) {
			pf_calibrate(&current_data, &current_cal, pf_get_parsed_file());
		}
		else {
			printf("no data to calibrate\n");
		}
	}
	else if (next_command == command_clear_calibration) {
		if (current_cal.valid) {
			printf("calibration cleared\n");
		}
		
		current_cal.valid = 0;
	}

	printf("command ok\n");
}

/**
 * The main function
 * @param[in] argc The number of command line arguments
 * @param[in] argv The command line arguments
 * @return The program exit status
 */
int 
main(int argc, char* argv[]) 
{
#ifndef HAVE_LIBREADLINE
	char cmdlinebuf[MAXCMDLINE];
#endif
	char* next_command;

	should_quit = 0;
	
	has_data = 0;
	current_cal.valid = 0;

#ifdef HAVE_READLINE_HISTORY
	using_history();
	read_history(HISTORY_FILE);
#endif

	pf_init_parse_data();

	printf("peakfinder, Copyright (C) 2005 Hal Finkel\n");
	printf("peakfinder comes with ABSOLUTELY NO WARRANTY.\n");

	next_command = NULL;
	do {
		if (next_command) {
			free(next_command);
		}

#ifdef HAVE_LIBREADLINE		
		next_command = readline("> ");
#else
		printf("> ");
		fgets(cmdlinebuf, MAXCMDLINE, stdin);
		next_command = strdup(cmdlinebuf);
#endif
	} while (pf_parse_command(next_command) && !should_quit);

#ifdef HAVE_READLINE_HISTORY
	if (write_history(HISTORY_FILE)) {
		perror("unable to save command history");
	}
#endif

	return 0;
}

