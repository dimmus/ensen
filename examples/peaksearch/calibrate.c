/**
 * @file calibrate.c
 * @author Hal Finkel
 *
 * Calibration and fitting functions
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

#include "ensen_private.h"
#include "signal_peaksearch_common.h"

/**
 * The maximum number of fitting iterations
 */
#define MAXITER 1000

/**
 * The maximum number of channels by which a peak location can vary from the fit
 */
#define MAXFLEX 6

/**
 * Data to which the calibration curve is fit
 */
struct cal_fit_info {
	int npts; /**< The number of points */
	double x[MAXCALPTS]; /**< The channel for each point */
	double x_err[MAXCALPTS]; /**< The error in the channel for each point */
	double y[MAXCALPTS]; /**< The calibrated value for each point */

	int pt[MAXCALPTS]; /**< The index of the point in the pf_cal_pts structure (ordered by value) */
	int pk[MAXCALPTS]; /**< The index of the peak in the pf_data structure */
};

/**
 * Evaulate the calibration function for the fit
 * @param[in] x The fit-parameter vector
 * @param[in] params A pointer to the cal_fit_info structure
 * @param[out] f The result of evaluating the function at the data points
 * @return GSL status code
 */
static int cal_f(const gsl_vector* x, void *params, gsl_vector * f) {
	int i, n;
	double a, b, c;
	struct cal_fit_info* data;

	data = (struct cal_fit_info* ) params;
	n = data->npts;

	a = gsl_vector_get(x, 0);
	b = gsl_vector_get(x, 1);
	c = gsl_vector_get(x, 2);

	for (i = 0; i < n; ++i) {
		double t, v;
		t = data->x[i];
		v = a*t*t + b*t + c;
		gsl_vector_set(f, i, (v - data->y[i])/data->x_err[i]);
	}

	return GSL_SUCCESS;
}

/**
 * Evaulate the Jacobian of the calibration function for the fit
 * @param[in] x The fit-parameter vector
 * @param[in] params A pointer to the cal_fit_info structure
 * @param[out] J The result of evaluating the Jacobian of the function at the data points
 * @return GSL status code
 */
static int cal_df(const gsl_vector * x ENSEN_UNUSED, void *params, gsl_matrix * J) {
	int i, n;
	struct cal_fit_info* data;

	data = (struct cal_fit_info* ) params;
	n = data->npts;
	
	/* NOT USED
	double a = gsl_vector_get(x, 0);
	double b = gsl_vector_get(x, 1);
	double c = gsl_vector_get(x, 2);
	*/

	for (i = 0; i < n; ++i) {
		double t, v_da, v_db, v_dc;
		t = data->x[i];
		
		v_da = t*t;
		v_db = t;
		v_dc = 1.0;
		
		gsl_matrix_set(J, i, 0, v_da/data->x_err[i]);
		gsl_matrix_set(J, i, 1, v_db/data->x_err[i]);
		gsl_matrix_set(J, i, 2, v_dc/data->x_err[i]);
	}

	return GSL_SUCCESS;
}

/**
 * Evaulate the calibration function and its Jacobian for the fit
 * @param[in] x The fit-parameter vector
 * @param[in] params A pointer to the cal_fit_info structure
 * @param[out] f The result of evaluating the function at the data points
 * @param[out] J The result of evaluating the Jacobian of the function at the data points
 * @return GSL status code
 */
static int cal_fdf(const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J) {
	cal_f(x, params, f);
	cal_df(x, params, J);

	return GSL_SUCCESS;
}

/**
 * qsort compar function to sort pf_cal_pt pointers by value
 * @param[in] ptr1 The first pointer
 * @param[in] ptr2 The second pointer
 * @return The difference between the value of the first point and the value of the second point
 */
static int compptvalue(const void* ptr1, const void* ptr2) {
	const struct pf_cal_pt* p1;
	const struct pf_cal_pt* p2;

	p1 = *((const struct pf_cal_pt**) ptr1);
	p2 = *((const struct pf_cal_pt**) ptr2);

	return (int) (p2->pt - p1->pt);
}

/**
 * qsort compar function to sort pf_cal_pt pointers by relative intensity
 * @param[in] ptr1 The first pointer
 * @param[in] ptr2 The second pointer
 * @return The difference between the relative intensity of the first point and the relative intensity of the second point
 */
static int compptri(const void* ptr1, const void* ptr2) {
	const struct pf_cal_pt* p1;
	const struct pf_cal_pt* p2;

	p1 = *((const struct pf_cal_pt**) ptr1);
	p2 = *((const struct pf_cal_pt**) ptr2);

	return (int) (p2->ri - p1->ri);
}

/**
 * Read the calibration points from a file
 * @param[in] fn The name of the file from which to read the calibration points
 * @param[out] p The calibration points structure
 * @return Boolean success flag
 */
static int readcalpts(char* fn, struct pf_cal_pts* p) {
	int i;
	FILE* fin;

	fin = fopen(fn, "r");
	if (!fin) {
		printf("unable to open %s\n", fn);
		return 0;
	}

	if (!fgets(p->unit, MAXUNIT, fin)) {
		p->unit[0] = '\0';
	}
	else {
		int sl;

		sl = strlen(p->unit);
		p->unit[sl-1] = '\0';
	}

	i = 0;
	while (fscanf(fin, "%lg %lg", &p->pts[i].pt, &p->pts[i].ri) == 2) {
		p->pts[i].num = i;

		p->pts_by_value[i] = &p->pts[i];
		p->pts_by_ri[i] = &p->pts[i];
		
		++i;
	}

	p->npts = i;

	qsort(p->pts_by_value, p->npts, sizeof(struct pf_cal_pt*), compptvalue);
	qsort(p->pts_by_ri, p->npts, sizeof(struct pf_cal_pt*), compptri);

	fclose(fin);

	return 1;
}

/**
 * Compute a linear fit using only two data points
 * @param[in] d The data set
 * @param[in] p The calibration data
 * @param[out] f The fit structure to initialize
 * @param[in] x1n The index (by height) of the peak of the first point
 * @param[in] x2n The index (by height) of the peak of the second point
 * @param[in] y1n The index (by value) of the calibration point of the first data point
 * @param[in] y2n The index (by value) of the calibration point of the second data point
 */
static int callinear(struct pf_data* d, struct pf_cal_pts* p, struct pf_cal_fit* f, int x1n, int x2n, int y1n, int y2n, int pnt) {
	double y1, y2, x1, x2, m, b;

	if (d->peaks_by_height[x2n]->fit.x0 < d->peaks_by_height[x1n]->fit.x0) {
		int xt;

		xt = x2n;
		x2n = x1n;
		x1n = xt;
	}

	y1 = p->pts_by_value[y1n]->pt;
	y2 = p->pts_by_value[y2n]->pt;

	x1 = d->peaks_by_height[x1n]->fit.x0;
	x2 = d->peaks_by_height[x2n]->fit.x0;

	if (pnt) {
		printf("using peak %d for %f\n", d->peaks_by_height[x2n]->num, y2);
		printf("using peak %d for %f\n", d->peaks_by_height[x1n]->num, y1);
	}

	m = (y2 - y1)/(x2 - x1);
	b = y2 - m*x2;

	f->a = 0;
	f->b = m;
	f->c = b;

	f->a_err = 0;
	f->b_err = 0;
	f->c_err = 0;

	f->chisq = 0;
	f->chisq_dof = 0;

	f->conv = 1;

	f->valid = 1;

	return 1;
}

/**
 * Calibrate using only two data points
 @param[in] d The data set
 @param[in] p The calibration points
 @param[out] f The calibration fit
 @return Boolean success flag
 
 With only two points, just pick the two largest peaks
 */
static int cal2pt(struct pf_data* d, struct pf_cal_pts* p, struct pf_cal_fit* f) {
	return callinear(d, p, f, 1, 0, 1, 0, 1);
}

/**
 * Find the peak closest to a given calibration value 
 * @param[in] d The data set
 * @param[in] f The calibration fit
 * @param[in] de The desired calibration value
 * @return The index of the closest peak
 */
static int peaknear(struct pf_data* d, struct pf_cal_fit* f, double de) {
	int i, n, high, low;
	double cehd, celd;

	n = d->npeaks;
	for (low = -1, high = n; high - low > 1; ) {
		double ce;
		
		i = (low + high) / 2;

		ce = pf_calfunc(f, d->peaks[i].cbin);
		if (de == ce) {
			return i;	
		}
		else if (de < ce) {
			high = i;
		}
		else {
			low = i;
		}
	}

	cehd = fabs(pf_calfunc(f, d->peaks[high].cbin) - de);
	celd = fabs(pf_calfunc(f, d->peaks[low].cbin) - de);

	if (cehd < celd || low < 0) {
		return high;
	}
	else {
		return low;
	}
}

/**
 * Find all peaks which match calibration points using the current calibration
 * @param[in] d The data set
 * @param[in] p The calibration points
 * @param[in] f The current calibration fir
 * @param[out] info Information on the peaks which matched calibration points using the old fit
 */
static void findpts(struct pf_data* d, struct pf_cal_pts* p, struct pf_cal_fit* f, struct cal_fit_info *info) {
	int i, lp;

	info->npts = 0;

	for (i = 0; i < p->npts; ++i) {
		double ubc, lbc, ub, lb;
	
		lp = peaknear(d, f, p->pts_by_value[i]->pt);

		ubc = d->peaks[lp].fit.x0 + MAXFLEX;
		if (ubc > NCHAN - 1) {
			ubc = NCHAN - 1;
		}

		lbc = d->peaks[lp].fit.x0 - MAXFLEX;
		if (lbc < 0) {
			lbc = 0;
		}
		
		ub = pf_calfunc(f, ubc);
		lb = pf_calfunc(f, lbc);
		if (p->pts_by_value[i]->pt >= lb && p->pts_by_value[i]->pt <= ub) {
			info->x[info->npts] = d->peaks[lp].fit.x0;
			info->x_err[info->npts] = d->peaks[lp].fit.x0_err;
			info->y[info->npts] = p->pts_by_value[i]->pt;

			info->pt[info->npts] = i;
			info->pk[info->npts] = lp;

			++info->npts;
		}
	}
}

/**
 * Get the best calibration fit to the set of matched peaks and calibration points
 * @param[in] info Information on the set of matched peaks and calibration points
 * @param[out] fit The new calibration fit
 */
static void fitcal(struct cal_fit_info* info, struct pf_cal_fit* fit) {
	const gsl_multifit_fdfsolver_type *T;
	gsl_multifit_fdfsolver *s;

	int iter, status;
	double chi;

	gsl_matrix *covar;
	gsl_multifit_function_fdf f;

	double c0, c1, cov00, cov01, cov11, sumsq;

	double ci[3];

	gsl_vector_view x;

	/* hint the fit with a linear fit */
	gsl_fit_linear(info->x, 1, info->y, 1, info->npts, &c0, &c1, &cov00, &cov01, &cov11, &sumsq);
	
	ci[0] = 0;
	ci[1] = c1;
	ci[2] = c0;

	covar = gsl_matrix_alloc(3, 3);

	x = gsl_vector_view_array(ci, 3);

	f.f = &cal_f;
	f.df = &cal_df;
	f.fdf = &cal_fdf;
	f.n = info->npts;
	f.p = 3;
	f.params = info;

	T = gsl_multifit_fdfsolver_lmsder;
	s = gsl_multifit_fdfsolver_alloc(T, info->npts, 3);
	gsl_multifit_fdfsolver_set (s, &f, &x.vector);

	iter = 0;
	do {
		++iter;

		status = gsl_multifit_fdfsolver_iterate(s);
		if (status) {
			break;
		}

		status = gsl_multifit_test_delta(s->dx, s->x, 1e-4, 1e-4);
	} while (status == GSL_CONTINUE && iter < MAXITER);

	if (status == GSL_CONTINUE) {
		fit->conv = 0;
	}
	else {
		fit->conv = 1;
	}

	// gsl_multifit_covar(s->J, 0.0, covar);

	fit->a = gsl_vector_get(s->x, 0);
	fit->b = gsl_vector_get(s->x, 1);
	fit->c = gsl_vector_get(s->x, 2);

	fit->a_err = sqrt(gsl_matrix_get(covar,0,0));
	fit->b_err = sqrt(gsl_matrix_get(covar,1,1));
	fit->c_err = sqrt(gsl_matrix_get(covar,2,2));

	chi = gsl_blas_dnrm2(s->f);
	fit->chisq = chi*chi;
	fit->chisq_dof = (chi*chi)/(info->npts - 3);

	fit->valid = 1;

	gsl_multifit_fdfsolver_free(s);
	gsl_matrix_free(covar);
}

/**
 * Print the list of matched peaks and calibration points
 * @param[in] d The data set
 * @param[in] p The calibration data
 * @param[in] info Information on matched peaks and calibration points
 */
static void printmatch(struct pf_data* d ENSEN_UNUSED, struct pf_cal_pts* p, struct cal_fit_info* info) {
	int i;

	for (i = 0; i < info->npts; ++i) {
		printf("using peak %d for %f\n", info->pk[i], p->pts_by_value[info->pt[i]]->pt);
	}
}

/**
 * Calibrate based on multiple calibration points
 * @param[in] d The data set
 * @param[in] p The calibration data
 * @param[out] f The new calibration fit
 * @return Boolean success flag
 */
static int calmpt(struct pf_data* d, struct pf_cal_pts* p, struct pf_cal_fit* f) {
	struct cal_fit_info info;

	if (f->valid) {
		findpts(d, p, f, &info);
		printmatch(d, p, &info);

		if (!info.npts) {
			printf("could not find a match for any new calibration point using the existing calibration\n");
			return 0;
		}

		if (info.npts == 1) {
			printf("only one new calibration point can be matched using the existing calibration, at least two are required\n");
			return 0;
		}

		fitcal(&info, f);
		return 1;
	}
	else {
		int fpt, spt, fpk, spk;

		for (fpt = 0; fpt < p->npts; ++fpt) {
			for (spt = fpt + 1; spt < p->npts; ++spt) {
				for (fpk = 0; fpk < d->npeaks; ++fpk) {
					for (spk = fpk + 1; spk < d->npeaks; ++spk) {
						/* this will prefer higher peaks as the linear fit uses peaks_by_height */
						callinear(d, p, f, spk, fpk, spt, fpt, 0);
						findpts(d, p, f, &info);

						if (info.npts == p->npts) {
							fitcal(&info, f);
							printmatch(d, p, &info);
							return 1;
						}
					}
				}
			}
		}

		printf("unable to match all calibration points\n");
		return 0;
	}
}

int pf_calibrate(struct pf_data* d, struct pf_cal_fit* f, char* fn) {
	struct pf_cal_pts p; 

	if (d->npeaks < 2) {
		printf("cannot calibrate with less than two peaks\n");
		return 0;
	}

	if (!readcalpts(fn, &p)) {
		return 0;
	}

	if (p.npts < 2) {
		printf("must have at least two calibration points\n");
		return 0;
	}
	else if (p.npts == 2) {
		cal2pt(d, &p, f);
	}
	else {
		calmpt(d, &p, f);
	}

	if (f->valid) {
		strncpy(f->unit, p.unit, MAXUNIT);
	}

	return 1;
}

double pf_calfunc(struct pf_cal_fit* f, double ch) {
	return f->a * ch*ch + f->b * ch + f->c;
}

