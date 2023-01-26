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
 * @file loaddata.c
 * @author Hal Finkel
 *
 * Data loading, normalization and background identification
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <gsl/gsl_statistics.h>
#include <gsl/gsl_fit.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>

#include "common.h"

/**
 * Minimum width of a peak
 *
 * This number must be greater than or equal to the number of parameters in the peak fit
 */
#define MINWIDTH 4

/**
 * Esitmate the initial average window
 * @param[in] d The data set
 * @return The initial average window size
 *
 * Estimate initial average window by inverse slope of linear data fit. A guess at the initial window is derived from the number of channels it takes for the average counts per bin to change by 1
 */
static int estavgwin(struct pf_data* d) {
	int i, as;
	double c0, c1, cov00, cov01, cov11, sumsq;
	double chn[NCHAN];

	for (i = 0; i < NCHAN; ++i) {
		chn[i] = i;
	}

	gsl_fit_linear(chn, 1, d->data_raw, 1, NCHAN, &c0, &c1, &cov00, &cov01, &cov11, &sumsq);
	
	if (c1 == 0) {
		as = NCHAN;
	}
	else {
		as = (int) floor(fabs(1.0/c1) + 0.5);
		if (as > NCHAN) {
			as = NCHAN;
		}
		if (as < 3) {
			printf("warning: large background slope\n");
			as = 3;
		}
	}

	return as;
}

/**
 * Determine if the current index is just after a local flat region
 * @param[in] d The data set
 * @param[in] i The index into the raw data
 * @return Boolean flag indicating if the current index is just after a local flat region
 */
static int pastdatabot(double* d, int i) {
	if (d[i] > 0) {
		if (d[i-1] == 0 && d[i-2] == 0) {
			return 1;
		}
	}

	if (d[i] == 0 && d[i-1] == 0) {
		if (d[i-2] > 0) {
			return 1;
		}
	}

	return 0;
}

/**
 * Determine if the current index is just after a local minimum
 * @param[in] d The data set
 * @param[in] i The index into the raw data
 * @return Boolean flag indicating if the current index is just after a local minimum
 */
static int pastdatamin(double* d, int i) {
	if (d[i] > d[i-1]) {
		if (d[i-1] < d[i-2]) {
			return 1;
		}
	}

	return pastdatabot(d, i);
}

/**
 * Determine if the current index is just after a local maximum
 * @param[in] d The data set
 * @param[in] i The index into the raw data
 * @return Boolean flag indicating if the current index is just after a local maximum
 */
static int pastdatamax(double* d, int i) {
	if (d[i] < d[i-1] && d[i-1] > d[i-2]) {
		return 1;
	}

	return 0;
}

/**
 * Determine if the candidate peak is statistically significant
 * @param[in] work The working raw-data array
 * @param[in] lbin The first data-point in the region
 * @param[in] cbin The center data-point in the region
 * @param[in] rbin The last data-point in the region
 * @return Boolean flag indicating statistical signifigance
 */
static int isstatsig(double* work, int lbin, int cbin, int rbin) {
	/* the number of counts in the peak height should be greater than the sqrt of the
		number of counts at the minimum, otherwise it is not
		statistically significant.
	*/

	/* require 2-sigma significance */

	if (work[cbin] - work[lbin] < 2*sqrt(work[lbin])) {
		return 0;
	}

	if (work[cbin] - work[rbin] < 2*sqrt(work[rbin])) {
		return 0;
	}

	return 1;
}

/**
 * Normalize the raw input data
 * @param[in,out] d The data set
 */
static void normdata(struct pf_data* d) {
	int i, as;
	int lmini, lmaxi;
	double work[NCHAN];

	for (i = 0; i < NCHAN; ++i) {
		d->data_bg[i] = 0;
		work[i] = d->data_raw[i];
	}

	as = estavgwin(d);
	printf("initial normalization window: %d\n", as);

	lmini = lmaxi = 0;

	d->npeaks = 0;

	for (i = 0; i < NCHAN; ++i) {
		double chexv, md, csv;
		
		chexv = pf_chanwindowavg(work, NCHAN, i, as);
		md = d->data_raw[i] - chexv;
		if (md < 0) {
			md = 0;
		}

		if (chexv == 0) {
			csv = 0;
		}
		else {
			csv = (md*md)/chexv;
		}
		
		d->data_norm[i] = csv;

		if (i > 1) {
			if (pastdatamax(d->data_norm, i)) {
				lmaxi = i-1;
			}
			else if (pastdatamin(d->data_norm, i)) {
				int cmini;

				cmini = i-1;

				if (lmini > 0 && lmaxi > lmini) {
					if (isstatsig(work, lmini, lmaxi, cmini) && (cmini - lmini + 1 >= MINWIDTH)) {
						int j;
						double m, b, c;

						/* past a peak, record and remove it */

						d->peaks[d->npeaks].lbin = lmini;
						d->peaks[d->npeaks].cbin = lmaxi;
						d->peaks[d->npeaks].rbin = cmini;

						if (d->npeaks > 0) {
							int nas, k;
							/* set the average window to twice the difference between this peak
								and the last peak
							*/
						
							nas = 2*(lmini - d->peaks[d->npeaks-1].rbin);
							if (nas > 2) {
								as = nas;
							}

							/* the space between this peak and the last is background */

							for (k = d->peaks[d->npeaks-1].rbin + 1; k < lmini - 1; ++k) {
								d->data_bg[k] = 1;
							}
						}
						else {
							int k;
							
							for (k = 0; k < lmini; ++k) {
								d->data_bg[k] = 1;	
							}
						}
						
						d->npeaks += 1;

						m = cmini - lmini;
						b = work[lmini];
						c = work[cmini];
						for (j = 0; j < m; ++j) {
							int wi;

							wi = j + lmini;
							work[wi] = (c-b)*j/m + b;
						}

						/* now recompute the normalization for the current point */
						--i;
					}
				}

				lmini = cmini;
			}
		}
	}

	d->total_chisq = 0;
	for (i = 0; i < NCHAN; ++i) {
		d->total_chisq += d->data_norm[i];
	}

	d->max_norm = gsl_stats_max(d->data_norm, 1, NCHAN);
	d->min_norm = gsl_stats_min(d->data_norm, 1, NCHAN);
}

int pf_loaddata(char* fn, struct pf_data* d) {
	int i, lch;
	double lv;
	FILE* fin;

	fin = fopen(fn, "r");
	if (!fin) {
		printf("unable to open %s\n", fn);
		return 0;
	}

	d->total_counts = 0;

	for (i = 0; i < NCHAN; ++i) {
		d->data_raw[i] = 0;
		d->data_norm[i] = 0;
	}

	while (fscanf(fin, "%d %lg", &lch, &lv) == 2) {
		if (lch >= NCHAN || lch < 0) {
			printf("invalid channel number: %d\n", lch);
			return 0;
		}

		d->data_raw[lch] = lv;
		d->total_counts += lv;
	}

	fclose(fin);

	printf("total counts: %d\n", d->total_counts);

	d->max_raw = gsl_stats_max(d->data_raw, 1, NCHAN);
	d->min_raw = gsl_stats_min(d->data_raw, 1, NCHAN);

	printf("min/max counts per bin: %g/%g\n", d->min_raw, d->max_raw);

	normdata(d);

	pf_fitpeaks(d);
	pf_prunepeaks(d);

	return 1;
}

double pf_chanwindowavg(double* data, int ds, int ci, int ac) {
	int nb, na, fb, nw;

	if (ac % 2 == 0) {
		ac += 1;
	}

	if (ac >= NCHAN) {
		ac = NCHAN - 1;
	}

	fb = 0;
	nb = na = (ac/2);
	if (ci < nb) {
		int d = nb - ci;
		nb -= d;
		na += d;
		fb = 1;
	}
	if (ci + na >= ds) {
		int d = (ci + nb) - ds + 1;
		na -= d;
		if (!fb) {
			nb += d;
		}
	}

	nw = nb + na + 1;
	return gsl_stats_mean(&data[ci - nb], 1, nw);
}

