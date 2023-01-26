/*
        peakfinder
        Copyright (C) 2005 Hal Finkel

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2 of the License, or
        (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program; if not, write to the Free Software
        Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 * @file commands.h
 * @author Hal Finkel
 *
 * Command lexing and parsing
 */

#include "common.h"

#ifndef PF_COMMANDS_H
#define PF_COMMANDS_H

/**
 * A command type
 */
enum pf_command {
	command_invalid, /**< Invalid command */
	command_quit, /**< Quit command */
	command_load, /**< Load command */
	command_print_data, /**< Print-data command */
	command_print_peaks, /**< Print-peaks command */
	command_print_peak, /**< Print-peak command */
	command_print_calibration, /**< Print-calibration command */
	command_plot_data, /**< Plot-data command */
	command_plot_peak, /**< Plot-peak command */
	command_calibrate, /**< Calibrate command */
	command_clear_calibration /**< Clear-calibration command */
};

/**
 * A plot-range specification 
 */
struct pf_plot_range {
	double xstart; /**< Starting domain value */
	double xend; /**< Ending domain value */

	int xstartcal; /**< Flag indicating if xstart is in calibrated units */
	int xendcal; /**< Flag indicating if xend is in calibrated units */
};

/**
 * Get the parsed command
 * @return The parsed command
 */
enum pf_command pf_get_parsed_command();

/**
 * Get the parsed sort flag
 * @return The parsed sort flag
 */
enum pf_peak_sort pf_get_parsed_sort();

/**
 * Get the parsed plot range
 * @return The parsed plot range
 */
struct pf_plot_range* pf_get_parsed_plot_range();

/**
 * Get the parsed file name
 * @return The parsed file name
 */
char* pf_get_parsed_file();

/**
 * Get the parsed driver name
 * @return The parsed driver name
 */
char* pf_get_parsed_driver();

/**
 * Get the parsed rescale flag
 * @return The parsed rescale flag
 */
int pf_get_parsed_rescale();

/**
 * Get the parsed calibration flag
 * @return The parsed calibration flag
 */
int pf_get_parsed_cal();

/**
 * Get the parsed normailzation flag
 * @return The parsed normailzation flag
 */
int pf_get_parsed_norm();

/**
 * Get the parsed mark flag
 * @return The parsed mark flag
 */
int pf_get_parsed_marked();

/**
 * Get the parsed annotate flag
 * @return The parsed annotate flag
 */
int pf_get_parsed_annot();

/**
 * Get the parsed peak number
 * @return The parsed peak number
 */
int pf_get_parsed_peak();

/**
 * The the parsed rotation amount
 * @return The parsed rotation amount
 */
double pf_get_parsed_rotation();

struct pf_plot_params;

/**
 * Get the parsed plot parameters
 * @param[out] p The plot parameters
 */
void pf_get_parsed_plot_params(struct pf_plot_params* p);

/**
 * Initialize the parse context data
 */
void pf_init_parse_data();

/**
 * Free any data associated with the last parse
 */
void pf_free_parse_data();

/**
 * Execute the next command
 */
void pf_execute_command();

/**
 * Set the buffer to parse
 * @param[in] cmd The buffer with the command string
 */
void pf_set_command_parse_buffer(char* cmd);

#endif
