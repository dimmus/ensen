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
 * @file common.h
 * @author Hal Finkel
 *
 * Main peakfinder program
 */

#ifndef PF_COMMON_H
#define PF_COMMON_H

/**
 * The maximum number of channels in the data set
 */
#define NCHAN 4096

/**
 * The maximum number of calibration points
 */
#define MAXCALPTS 1024

/**
 * The maximum length of a calibration unit string
 */
#define MAXUNIT 256

/**
 * Peak background fit
 */
struct pf_peak_bg {
	double a; /**< Coefficient of the cubic term */
	double b; /**< Coefficient of the quadradic term */
	double c; /**< Coefficient of the linear term */
	double d; /**< The constant term */

	double a_err; /**< Error in a */
	double b_err; /**< Error in b */
	double c_err; /**< Error in c */
	double d_err; /**< Error in d */

	double chisq; /**< chi^2 of the fit */
	double chisq_dof; /**< chi^2 per degree of freedom */

	int conv; /**< Boolean convergance flag */
};

/**
 * Peak fit
 */
struct pf_peak_fit {
	double a; /**< The a parameter */
	double k; /**< The k parameter */
	double x0; /**< The x0 parameter */
	double m; /**< The m parameter */

	double a_err; /**< Error in a */
	double k_err; /**< Error in k */
	double x0_err; /**< Error in x0 */
	double m_err; /**< Error in m */

	double chisq; /**< chi^2 of the fit */
	double chisq_dof; /**< chi^2 per degree of freedom */

	int conv; /**< Boolean convergance flag */
};

/**
 * Peak data
 */
struct pf_peak {
	int num; /**< Index of the peak */

	int lbin; /**< Starting bin of the peak region */
	int cbin; /**< Center (maximum) bin of the peak region */
	int rbin; /**< Ending bin of the peak region */

	struct pf_peak_bg bg; /**< Background fit */
	struct pf_peak_fit fit; /**< Peak fit */

	double center; /**< Peak center location */
	double width; /**< Peak FWHM */

	double lbound; /**< Starting value of the peak */
	double rbound; /**< Ending value of the peak */

	double bglbound; /**< Starting value of the peak background */
	double bgrbound; /**< Ending value of the peak background */

	double area; /**< Total peak area */
	double height; /**< Maximum peak height */
};

/**
 * Calibration fit
 */
struct pf_cal_fit {
	char unit[MAXUNIT]; /**< The name of the calibration unit */

	double a; /**< The quadradic coefficient */
	double b; /**< The linear coefficient */
	double c; /**< The constant term */

	double a_err; /**< Error in a */
	double b_err; /**< Error in b */
	double c_err; /**< Error in c */

	double chisq; /**< chi^2 of the fit */
	double chisq_dof; /**< chi^2 per degree of freedom */

	int conv; /**< Boolean convergance flag */

	int valid; /**< Boolean validity flag */
};

/**
 * Calibration point
 */
struct pf_cal_pt {
	int num; /**< Index of the calibration point */
	double pt; /**< The value of the point */
	double ri; /**< The relative intensity of the point */
};

/**
 * Calibration points
 */
struct pf_cal_pts {
	char unit[MAXUNIT]; /**< The name of the calibration unit */

	int npts; /**< The number of calibration points */
	struct pf_cal_pt pts[MAXCALPTS]; /**< The calibration points */
	struct pf_cal_pt* pts_by_value[MAXCALPTS]; /**< Calibration points sorted by value */
	struct pf_cal_pt* pts_by_ri[MAXCALPTS]; /**< Calibration points sorted by relative intensity */
};

/**
 * Data set
 */
struct pf_data {
	double data_raw[NCHAN]; /**< Raw data */
	double data_norm[NCHAN]; /**< Normalized data */

	double max_raw; /**< Maximum raw value */
	double min_raw; /**< Minimum raw value */
	double max_norm; /**< Maximum normalized value */
	double min_norm; /**< Minimum normalized value */

	int total_counts; /**< Total number of raw counts */
	double total_chisq; /**< Total number of normalized counts */

	int data_bg[NCHAN]; /**< Bin background marking */

	int npeaks; /**< The number of peaks */
	struct pf_peak peaks[NCHAN]; /**< Identified peaks */
	struct pf_peak* peaks_by_number[NCHAN]; /**< Peaks sorted by number */
	struct pf_peak* peaks_by_height[NCHAN]; /**< Peaks sorted by height */
	struct pf_peak* peaks_by_area[NCHAN]; /**< Peaks sorted by area */
};

/**
 * Plot parameters
 */
struct pf_plot_params {
	double xstart; /**< Starting domain value */
	double xend; /**< Ending domain value */

	int xstartcal; /**< Flag indicating if xstart is in calibrated units */
	int xendcal; /**< Flag indicating if xend is in calibrated units */

	int peak; /**< Peak to plot */

	char* fn; /**< Output file name */
	char* dev; /**< Plotting device name */
	
	double rot; /**< Number of degrees to rotate the plot */
	int norm; /**< Flag indicating whether or not to plot normalized values */
	int rescale; /**< Flag indicating whether or not to rescale plot */
	int cal; /**< Flag indicating whether or not to plot in calibrated units */
	int marked; /**< Flag indicating whether or not to mark non-backgound regions */
	int annot; /**< Flag indicating whether or not to annotate plot with peak numbers */
};

/**
 * Peak sort type
 */
enum pf_peak_sort {
	sort_number, /**< Sort by peak number */
	sort_height, /**< Sort by peak height */
	sort_area /**< Sort by peak area */
};

/**
 * Parse a command string
 * @param[in] command The command string to parse
 * @return Boolean status flag
 */
int pf_parse_command(char* command);

/**
 * Load data from a file
 * @param[in] fn The file name
 * @param[out] d The data set
 * @return Boolean status flag
 */
int pf_loaddata(char* fn, struct pf_data* d);

/**
 * Compute the average counts per bin over some window
 * @param[in] data The data array
 * @param[in] ds The size of the data array
 * @param[in] ci The center of the window
 * @param[in] ac The size of the window
 * @return The average over the window
 */
double pf_chanwindowavg(double* data, int ds, int ci, int ac);

/**
 * Plot data
 * @param[in] d The data set
 * @param[in] p The plotting parameters
 * @param[in] cal The calibration fit
 */
void pf_plotdata(struct pf_data* d, struct pf_plot_params* p, struct pf_cal_fit* cal);

/**
 * Plot a given peak
 * @param[in] d The data set
 * @param[in] p The plotting parameters
 * @param[in] cal The calibration fit
 */
void pf_plotpeak(struct pf_data* d, struct pf_plot_params* p, struct pf_cal_fit* cal);

/**
 * Print the data set
 * @param[in] d The data set
 * @param[in] fn The output file name
 * @param[in] norm Flag indicating whether to print normalized data
 * @return Boolean status flag
 */
int pf_printdata(struct pf_data* d, char* fn, int norm);

/**
 * Print the peaks
 * @param[in] d The data set
 * @param[in] fn The output file name
 * @param[in] srt Peak sort type
 * @return Boolean status flag
 */
int pf_printpeaks(struct pf_data* d, char* fn, enum pf_peak_sort srt);

/**
 * Print data about a given peak
 * @param[in] d The data set
 * @param[in] cal The calibration fit
 * @param[in] peak The peak about which to print data
 * @param[in] fn The output file name
 * @return Boolean status flag
 */
int pf_printpeak(struct pf_data* d, struct pf_cal_fit* cal, int peak, char* fn);

/**
 * Print data about the calibration fit
 * @param[in] d The data set
 * @param[in] f The calibration fit
 * @param[in] fn The output file name
 * @return Boolean status flag
 */
int pf_printcal(struct pf_data* d, struct pf_cal_fit* f, char* fn);

/**
 * Fit peaks in the data set
 * @param[in,out] d The data set
 */
void pf_fitpeaks(struct pf_data* d);

/**
 * Prune peaks without reasonable fits
 * @param[in,out] d The data set
 */
void pf_prunepeaks(struct pf_data* d);

/**
 * Evaluate the background function for a given peak
 * @param[in] d The data set
 * @param[in] peak The peak for which to evalutate the background function
 * @param[in] x The value at which to evaluate the function
 * @return The value of the function
 */
double pf_bgfunc(struct pf_data* d, int peak, double x);

/**
 * Evaluate the peak fit function for a given peak
 * @param[in] d The data set
 * @param[in] peak The peak for which to evalutate the peak fit function
 * @param[in] x The value at which to evaluate the function
 * @return The value of the function
 */
double pf_peakfunc(struct pf_data* d, int peak, double x);

/**
 * Evaluate the total fit function for a given peak
 * @param[in] d The data set
 * @param[in] peak The peak for which to evalutate the total fit function
 * @param[in] x The value at which to evaluate the function
 * @return The value of the function
 */
double pf_fitfunc(struct pf_data* d, int peak, double x);

/**
 * Calibrate input using known data points
 * @param[in] d The data set
 * @param[out] f The calibration fit
 * @param[in] fn The file containing the calibration points
 * @return Boolean success flag
 */
int pf_calibrate(struct pf_data* d, struct pf_cal_fit* f, char* fn);

/**
 * Evaluate the calibration fit function for a given peak
 * @param[in] f The calibration fit
 * @param[in] ch The value at which to evaluate the function
 * @return The value of the function
 */
double pf_calfunc(struct pf_cal_fit* f, double ch);

#endif

