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

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "commands.h"
#include "common.h"

int pf_printdata(struct pf_data* d, char* fn, int norm) {
	int i;
	FILE* of;

	if (fn) {
		of = fopen(fn, "w");
		if (!of) {
			printf("unable to open file: %s\n", fn);
			return 0;
		}
	}
	else {
		of = stdout;
	}

	for (i = 0; i < NCHAN; ++i) {
		double* od;

		if (norm) {
			od = d->data_norm;
		}
		else {
			od = d->data_raw;
		}
	
		fprintf(of, "%d %.10f\n", i, od[i]);
	}

	if (fn) {
		fclose(of);
	}

	return 1;
}

int pf_printpeaks(struct pf_data* d, char* fn, enum pf_peak_sort srt) {
	int i;
	struct pf_peak** pp;
	FILE* of;

	if (fn) {
		of = fopen(fn, "w");
		if (!of) {
			printf("unable to open file: %s\n", fn);
			return 0;
		}
	}
	else {
		of = stdout;
	}

	if (srt == sort_height) {
		pp = d->peaks_by_height;
	}
	else if (srt == sort_area) {
		pp = d->peaks_by_area;
	}
	else {
		pp = d->peaks_by_number;
	}

	for (i = 0; i < d->npeaks; ++i) {
		fprintf(of, "%d %d %d %d\n", pp[i]->num, pp[i]->cbin, pp[i]->lbin, pp[i]->rbin);
	}

	if (fn) {
		fclose(of);
	}

	return 1;
}

int pf_printpeak(struct pf_data* d, struct pf_cal_fit* cal, int peak, char* fn) {
	FILE* of;

	if (fn) {
		of = fopen(fn, "w");
		if (!of) {
			printf("unable to open file: %s\n", fn);
			return 0;
		}
	}
	else {
		of = stdout;
	}

	if (peak >= d->npeaks) {
		printf("valid peak numbers are: 0-%d\n", d->npeaks - 1);
		return 0;
	}
	if (peak < 0) {
		peak = 0;
	}

	fprintf(of, "peak %d\n", peak);
	fprintf(of, "peak bin: %d\n", d->peaks[peak].cbin);
	fprintf(of, "left bin: %d\n", d->peaks[peak].lbin);
	fprintf(of, "right bin: %d\n", d->peaks[peak].rbin);

	fprintf(of, "\n");

	fprintf(of, "peak center: %f\n", d->peaks[peak].center);
	fprintf(of, "peak apx. width: %f\n", d->peaks[peak].width);
	fprintf(of, "peak left bound: %f\n", d->peaks[peak].lbound);
	fprintf(of, "peak right bound: %f\n", d->peaks[peak].rbound);
	fprintf(of, "peak area: %f\n", d->peaks[peak].area);
	fprintf(of, "peak height: %f\n", d->peaks[peak].height);

	if (cal->valid) {
		fprintf(of, "\n");
		fprintf(of, "calibrated to: %f %s\n", pf_calfunc(cal, d->peaks[peak].fit.x0), cal->unit);
	}

	fprintf(of, "\n");

	fprintf(of, "background fit ax^3 + bx^2 + cx + d\n");
	fprintf(of, "a: %f +- %f\n", d->peaks[peak].bg.a, d->peaks[peak].bg.a_err);
	fprintf(of, "b: %f +- %f\n", d->peaks[peak].bg.b, d->peaks[peak].bg.b_err);
	fprintf(of, "c: %f +- %f\n", d->peaks[peak].bg.c, d->peaks[peak].bg.c_err);
	fprintf(of, "d: %f +- %f\n", d->peaks[peak].bg.d, d->peaks[peak].bg.d_err);
	fprintf(of, "chi-squared: %f\n", d->peaks[peak].bg.chisq);
	fprintf(of, "chi-squared/dof: %f\n", d->peaks[peak].bg.chisq_dof);
	fprintf(of, "fit left bound: %f\n", d->peaks[peak].bglbound);
	fprintf(of, "fit right bound: %f\n", d->peaks[peak].bgrbound);
	if (!d->peaks[peak].bg.conv) {
		fprintf(of, "fit did not converge\n");
	}

	fprintf(of, "\n");

	fprintf(of, "peak fit a(1 + ((k^2)(x - x0)^2)/m)^(-m)\n");
	fprintf(of, "a: %f +- %f\n", d->peaks[peak].fit.a, d->peaks[peak].fit.a_err);
	fprintf(of, "k: %f +- %f\n", d->peaks[peak].fit.k, d->peaks[peak].fit.k_err);
	fprintf(of, "x0: %f +- %f\n", d->peaks[peak].fit.x0, d->peaks[peak].fit.x0_err);
	fprintf(of, "m: %f +- %f\n", d->peaks[peak].fit.m, d->peaks[peak].fit.m_err);
	fprintf(of, "chi-squared: %f\n", d->peaks[peak].fit.chisq);
	fprintf(of, "chi-squared/dof: %f\n", d->peaks[peak].fit.chisq_dof);
	if (!d->peaks[peak].fit.conv) {
		fprintf(of, "fit did not converge\n");
	}

	if (fn) {
		fclose(of);
	}

	return 1;
}

int pf_printcal(struct pf_data* d, struct pf_cal_fit* f, char* fn) {
	FILE* of;

	if (!f->valid) {
		return 0;
	}

	if (fn) {
		of = fopen(fn, "w");
		if (!of) {
			printf("unable to open file: %s\n", fn);
			return 0;
		}
	}
	else {
		of = stdout;
	}

	fprintf(of, "calibration fit ax^2 + bx + c\n");
	fprintf(of, "a: %f +- %f\n", f->a, f->a_err);
	fprintf(of, "b: %f +- %f\n", f->b, f->b_err);
	fprintf(of, "c: %f +- %f\n", f->c, f->c_err);
	fprintf(of, "chi-squared: %f\n", f->chisq);
	fprintf(of, "chi-squared/dof: %f\n", f->chisq_dof);
	if (f->unit[0]) {
		fprintf(of, "units: %s\n", f->unit);
	}
	if (!f->conv) {
		fprintf(of, "fit did not converge\n");
	}

	if (fn) {
		fclose(of);
	}

	return 1;
}

