/**
 * @file peakfit.c
 * @author Hal Finkel
 *
 * Peak identification and fitting
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

#define MAXITER 100000 /** The maximum number of iterations for fitting peaks and background */
#define MINBGPTS 3 /** Mininum number of background points per side. Total from both sides needs to be at least the number of fit parameters */
#define BGWIDTH 2 /** The number of peak widths to look for background if possible  */
#define SEARCHSTEP 0.01 /** Search step for peak bounds */

/**
 * Data on the background for fitting
 */
struct bg_poly_data {
	int peak; /**< The index of the peak for which to fit background */
	int npts; /**< The number of background points */
	int pts[NCHAN]; /**< The list of bins associated with the background */
	struct pf_data* d; /**< The data set */
};

/**
 * Data on a peak for fitting
 */
struct peak_pearson7_data {
	int peak; /**< The index of the peak to fit */
	struct pf_data* d; /**< The data set */
};

/**
 * Compute the background polynomial
 * @param[in] a The coefficient of the cubic term
 * @param[in] b The coefficient of the quadradic term
 * @param[in] c The coefficient of the linear term
 * @param[in] d The constant term
 * @param[in] x The value at which to evalutate the function
 * @return The value of the function
 */
static double polyb(double a, double b, double c, double d, double x) {
	return a*x*x*x + b*x*x + c*x + d;
}

/**
 * Compute the derivative of the background polynomial with respect to a
 * @param[in] a The coefficient of the cubic term
 * @param[in] b The coefficient of the quadradic term
 * @param[in] c The coefficient of the linear term
 * @param[in] d The constant term
 * @param[in] x The value at which to evalutate the function
 * @return The value of the function
 */
static double polyb_da(double a ENSEN_UNUSED, double b ENSEN_UNUSED, double c ENSEN_UNUSED, double d ENSEN_UNUSED, double x) {
	return x*x*x;
}

/**
 * Compute the derivative of the background polynomial with respect to b
 * @param[in] a The coefficient of the cubic term
 * @param[in] b The coefficient of the quadradic term
 * @param[in] c The coefficient of the linear term
 * @param[in] d The constant term
 * @param[in] x The value at which to evalutate the function
 * @return The value of the function
 */
static double polyb_db(double a ENSEN_UNUSED, double b ENSEN_UNUSED, double c ENSEN_UNUSED, double d ENSEN_UNUSED, double x) {
	return x*x;
}

/**
 * Compute the derivative of the background polynomial with respect to c
 * @param[in] a The coefficient of the cubic term
 * @param[in] b The coefficient of the quadradic term
 * @param[in] c The coefficient of the linear term
 * @param[in] d The constant term
 * @param[in] x The value at which to evalutate the function
 * @return The value of the function
 */
static double polyb_dc(double a ENSEN_UNUSED, double b ENSEN_UNUSED, double c ENSEN_UNUSED, double d ENSEN_UNUSED, double x) {
	return x;
}

/**
 * Compute the derivative of the background polynomial with respect to d
 * @param[in] a The coefficient of the cubic term
 * @param[in] b The coefficient of the quadradic term
 * @param[in] c The coefficient of the linear term
 * @param[in] d The constant term
 * @param[in] x The value at which to evalutate the function
 * @return The value of the function
 */
static double polyb_dd(double a ENSEN_UNUSED, double b ENSEN_UNUSED, double c ENSEN_UNUSED, double d ENSEN_UNUSED, double x ENSEN_UNUSED) {
	return 1;
}

/**
 * Compute the Pearson VII function FWHM
 * @param[in] a The parameter a
 * @param[in] k The parameter k
 * @param[in] m The parameter m
 * @return The FWHM of the function
 */
static double pearson7_fwhm(double a ENSEN_UNUSED, double k, double m) {
	return 2.0*sqrt((m/(k*k))*(pow(2.0, 1.0/m) - 1.0));
}

/**
 * Compute the Pearson VII function parameter k for a given FWHM
 * @param[in] a The parameter a
 * @param[in] m The parameter m
 * @param[in] w The desired FWHM
 * @return The k parameter of the function
 */
static double pearson7_k_from_fwhm(double a ENSEN_UNUSED, double m, double w) {
	return 2.0*sqrt((m/(0.25*w*w))*(pow(2.0, 1.0/m) - 1.0));
}

/**
 * Compute the Pearson VII function
 * @param[in] a The parameter a
 * @param[in] k The parameter k
 * @param[in] x0 The parameter x0
 * @param[in] m The parameter m
 * @param[in] x The value at which to evalutate the function
 * @return The value of the function
 */
static double pearson7(double a ENSEN_UNUSED, double k, double x0, double m, double x) {
	double x1;

	x1 = x - x0;
	return a*pow(1.0 + (k*k*x1*x1)/m, -m);
}

/**
 * Compute the derivative of the Pearson VII function with respect to a
 * @param[in] a The parameter a
 * @param[in] k The parameter k
 * @param[in] x0 The parameter x0
 * @param[in] m The parameter m
 * @param[in] x The value at which to evalutate the function
 * @return The value of the function
 */
static double pearson7_da(double a ENSEN_UNUSED, double k, double x0, double m, double x) {
	double x1;

	x1 = x - x0;
	return pow(1.0 + (k*k*x1*x1)/m, -m);
}

/**
 * Compute the derivative of the Pearson VII function with respect to k
 * @param[in] a The parameter a
 * @param[in] k The parameter k
 * @param[in] x0 The parameter x0
 * @param[in] m The parameter m
 * @param[in] x The value at which to evalutate the function
 * @return The value of the function
 */
static double pearson7_dk(double a, double k, double x0, double m, double x) {
	double x1, p7;

	x1 = x - x0;
	p7 = pearson7(a, k, x0, m, x);
	return -2.0*(p7*k*x1*x1)/(1.0 + (k*k*x1*x1)/m);
}

/**
 * Compute the derivative of the Pearson VII function with respect to x0
 * @param[in] a The parameter a
 * @param[in] k The parameter k
 * @param[in] x0 The parameter x0
 * @param[in] m The parameter m
 * @param[in] x The value at which to evalutate the function
 * @return The value of the function
 */
static double pearson7_dx0(double a, double k, double x0, double m, double x) {
	double x1, p7;

	x1 = x - x0;
	p7 = pearson7(a, k, x0, m, x);
	return 2.0*(p7*k*k*x1)/(1.0 + (k*k*x1*x1)/m);
}

/**
 * Compute the derivative of the Pearson VII function with respect to m
 * @param[in] a The parameter a
 * @param[in] k The parameter k
 * @param[in] x0 The parameter x0
 * @param[in] m The parameter m
 * @param[in] x The value at which to evalutate the function
 * @return The value of the function
 */
static double pearson7_dm(double a, double k, double x0, double m, double x) {
	double x1, p7, i1;

	x1 = x - x0;
	p7 = pearson7(a, k, x0, m, x);
	i1 = 1.0 + (k*k*x1*x1)/m;
	return p7*((k*k*x1*x1)/(m*i1) - log(i1));
}

/**
 * Scale the differnce between the fit and the real data
 * @param[in] fv The fit value
 * @param[in] rv The real value
 * @return The scaled difference
 *
 * Scale sigma to be the sqrt of the real number of counts
 */
static double scaleerr(double fv, double rv) {
	if (rv <= 0) {
		return fv - rv;
	}
	
	return (fv - rv)/sqrt(rv);
}

/**
 * Scale the derivative value to account for the error scaling
 * @param[in] fv The derativive value
 * @param[in] rv The real value
 * @return The scaled deravitive
 */
static double scalederiv(double fd, double rv) {
	if (rv <= 0) {
		return fd;
	}
	
	return fd/sqrt(rv);
}

/**
 * Evaulate the background function for the fit
 * @param[in] x The fit-parameter vector
 * @param[in] params A pointer to the bg_poly_data structure
 * @param[out] f The result of evaluating the function at the data points
 * @return GSL status code
 */
static int polyb_f(const gsl_vector * x, void *params, gsl_vector * f) {
	int i;
	double a, b, c, d;
	struct bg_poly_data* data;
	
	data = (struct bg_poly_data*) params;
	a = gsl_vector_get(x, 0);
	b = gsl_vector_get(x, 1);
	c = gsl_vector_get(x, 2);
	d = gsl_vector_get(x, 3);

	for (i = 0; i < data->npts; ++i) {
		int ri;
		double t, rt;

		ri = data->pts[i];
		rt = data->d->data_raw[ri];

		t = ri;

		gsl_vector_set(f, i, scaleerr(polyb(a, b, c, d, t), rt));
	}

	return GSL_SUCCESS;
}

/**
 * Evaulate the Jacobian of the background function for the fit
 * @param[in] x The fit-parameter vector
 * @param[in] params A pointer to the bg_poly_data structure
 * @param[out] J The result of evaluating the Jacobian of the function at the data points
 * @return GSL status code
 */
static int polyb_df(const gsl_vector * x, void *params, gsl_matrix * J) {
	int i;
	double a, b, c, d;
	struct bg_poly_data* data;
	
	data = (struct bg_poly_data*) params;
	a = gsl_vector_get(x, 0);
	b = gsl_vector_get(x, 1);
	c = gsl_vector_get(x, 2);
	d = gsl_vector_get(x, 3);

	for (i = 0; i < data->npts; ++i) {
		int ri;
		double t, rt;

		ri = data->pts[i];
		rt = data->d->data_raw[ri];

		t = ri;

		gsl_matrix_set(J, i, 0, scalederiv(polyb_da(a, b, c, d, t), rt));
		gsl_matrix_set(J, i, 1, scalederiv(polyb_db(a, b, c, d, t), rt));
		gsl_matrix_set(J, i, 2, scalederiv(polyb_dc(a, b, c, d, t), rt));
		gsl_matrix_set(J, i, 3, scalederiv(polyb_dd(a, b, c, d, t), rt));
	}

	return GSL_SUCCESS;
}

/**
 * Evaulate the background function and its Jacobian for the fit
 * @param[in] x The fit-parameter vector
 * @param[in] params A pointer to the bg_poly_data structure
 * @param[out] f The result of evaluating the function at the data points
 * @param[out] J The result of evaluating the Jacobian of the function at the data points
 * @return GSL status code
 */
static int polyb_fdf(const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J) {
	polyb_f(x, params, f);
	polyb_df(x, params, J);

	return GSL_SUCCESS;
}

/**
 * Evaulate the Pearson VII function for the fit
 * @param[in] x The fit-parameter vector
 * @param[in] params A pointer to the peak_pearson7_data structure
 * @param[out] f The result of evaluating the function at the data points
 * @return GSL status code
 */
static int pearson7_f(const gsl_vector * x, void *params, gsl_vector * f) {
	int i, pw;
	double a, k, x0, m;
	struct peak_pearson7_data* data;
	
	data = (struct peak_pearson7_data*) params;
	a = gsl_vector_get(x, 0);
	k = gsl_vector_get(x, 1);
	x0 = gsl_vector_get(x, 2);
	m = gsl_vector_get(x, 3);

	pw = data->d->peaks[data->peak].rbin - data->d->peaks[data->peak].lbin + 1;

	for (i = 0; i < pw; ++i) {
		int ri;
		double t, rt, bg;

		ri = data->d->peaks[data->peak].lbin + i;
		rt = data->d->data_raw[ri];

		t = ri;

		bg = pf_bgfunc(data->d, data->peak, t);
		gsl_vector_set(f, i, scaleerr(pearson7(a, k, x0, m, t) + bg, rt));
	}

	return GSL_SUCCESS;
}

/**
 * Evaulate the Jacobian of the Pearson VII function for the fit
 * @param[in] x The fit-parameter vector
 * @param[in] params A pointer to the peak_pearson7_data structure
 * @param[out] J The result of evaluating the Jacobian of the function at the data points
 * @return GSL status code
 */
static int pearson7_df(const gsl_vector * x, void *params, gsl_matrix * J) {
	int i, pw;
	double a, k, x0, m;
	struct peak_pearson7_data* data;
	
	data = (struct peak_pearson7_data*) params;
	a = gsl_vector_get(x, 0);
	k = gsl_vector_get(x, 1);
	x0 = gsl_vector_get(x, 2);
	m = gsl_vector_get(x, 3);

	pw = data->d->peaks[data->peak].rbin - data->d->peaks[data->peak].lbin + 1;

	for (i = 0; i < pw; ++i) {
		int ri;
		double t, rt;

		ri = data->d->peaks[data->peak].lbin + i;
		rt = data->d->data_raw[ri];

		t = ri;

		gsl_matrix_set(J, i, 0, scalederiv(pearson7_da(a, k, x0, m, t), rt));
		gsl_matrix_set(J, i, 1, scalederiv(pearson7_dk(a, k, x0, m, t), rt));
		gsl_matrix_set(J, i, 2, scalederiv(pearson7_dx0(a, k, x0, m, t), rt));
		gsl_matrix_set(J, i, 3, scalederiv(pearson7_dm(a, k, x0, m, t), rt));
	}

	return GSL_SUCCESS;
}

/**
 * Evaulate the Pearson VII function and its Jacobian for the fit
 * @param[in] x The fit-parameter vector
 * @param[in] params A pointer to the peak_pearson7_data structure
 * @param[out] f The result of evaluating the function at the data points
 * @param[out] J The result of evaluating the Jacobian of the function at the data points
 * @return GSL status code
 */
static int pearson7_fdf(const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J) {
	pearson7_f(x, params, f);
	pearson7_df(x, params, J);

	return GSL_SUCCESS;
}

/**
 * Prepare for the background fit with a linear fit
 * @param[in] d The data set
 * @param[in] peak The index of the peak to fit
 * @param[in] pd Background points to fit
 * @param[out] ic The fit linear term
 * @param[out] id The fit constant term
 */
static void prepbgfit(struct pf_data* d, int peak ENSEN_UNUSED, struct bg_poly_data* pd, double* ic, double* id) {
	int i;
	double c0, c1, cov00, cov01, cov11, sumsq;
	double chn[NCHAN], chd[NCHAN];

	for (i = 0; i < pd->npts; ++i) {
		chn[i] = pd->pts[i];
		chd[i] = d->data_raw[pd->pts[i]];
	}

	gsl_fit_linear(chn, 1, chd, 1, pd->npts, &c0, &c1, &cov00, &cov01, &cov11, &sumsq);
	
	*ic = c1;
	*id = c0;
}

/**
 * Prepare for a peak fit with initial parameters
 * @param[in] d The data set
 * @param[in] peak The index of the peak to fit
 * @param[out] ia Initial parameter a
 * @param[out] ik Initial parameter k
 * @param[out] ix0 Initial parameter x0
 * @param[out] im Initial parameter m
 */
static void preppeakfit(struct pf_data* d, int peak, double* ia, double* ik, double* ix0, double* im) {
	double pw, bgh, ph;

	pw = d->peaks[peak].rbin - d->peaks[peak].lbin + 1.0;
	bgh = pf_bgfunc(d, peak, d->peaks[peak].cbin + 0.5);
	ph = d->data_raw[d->peaks[peak].cbin] - bgh;

	*im = 1.0;
	*ia = ph;
	*ix0 = d->peaks[peak].cbin + 0.5;
	*ik = pearson7_k_from_fwhm(ph, *im, pw);
}

/**
 * Select background points around a given peak
 * @param[in] d The data set
 * @param[in] peak The index of peak for which the background must be determined
 * @param[out] pd The background points selected
 * @param[out] lbound The starting bin index of the background points
 * @param[out] rbound The ending bin index of the background points
 */
static void prepbgpts(struct pf_data* d, int peak, struct bg_poly_data* pd, double* lbound, double* rbound) {
	int i, bgl1, bgl2, bgr1, bgr2, tl, tr;
	int pwidth, minlbin, maxrbin;

	/* fit the background using up to 1 peak width of background on either side of the peak */

	pd->d = d;
	pd->peak = peak;

	/* first, find where the background begins */

	for (bgl2 = d->peaks[peak].lbin - 1; bgl2 >= 0 && !d->data_bg[bgl2]; --bgl2);
	if (bgl2 < 0) {
		bgl2 = 0;
	}

	for (bgr1 = d->peaks[peak].rbin + 1; bgr1 < NCHAN && !d->data_bg[bgr1]; ++bgr1);
	if (bgr1 >= NCHAN) {
		bgr1 = NCHAN - 1;
	}

	/* determine the range of background to fit */

	pwidth = d->peaks[peak].rbin - d->peaks[peak].lbin + 1;
	minlbin = bgl2 - BGWIDTH*pwidth;
	maxrbin = bgr1 + BGWIDTH*pwidth;

	if (minlbin < 0) {
		minlbin = 0;
	}
	if (maxrbin >= NCHAN) {
		maxrbin = NCHAN - 1;
	}

	/* starting left and right total are 1 from above */
	tl = tr = 1;

	for (bgl1 = bgl2; (bgl1 >= minlbin && d->data_bg[bgl1]) || (bgl1 >= 0 && tl < MINBGPTS); --bgl1) {
		if (d->data_bg[bgl1]) {
			++tl;
		}
	}
	++bgl1;

	for (bgr2 = bgr1; (bgr2 <= maxrbin && d->data_bg[bgr2]) || (bgr2 < NCHAN && tr < MINBGPTS); ++bgr2) {
		if (d->data_bg[bgr2]) {
			++tr;
		}
	}
	--bgr2;
	
	/* BUG: we could still have not enough points, then what do we do? */

	/* put background channels into fit structure */

	pd->npts = 0;

	for (i = bgl1; i <= bgl2; ++i) {
		if (d->data_bg[i]) {
			pd->pts[pd->npts++] = i;
		}
	}
	for (i = bgr1; i <= bgr2; ++i) {
		if (d->data_bg[i]) {
			pd->pts[pd->npts++] = i;
		}
	}

	*lbound = bgl1;
	*rbound = bgr2;
}

/**
 * Fit the background of a given peak
 * @param[in,out] d The data set
 * @param[in] The index of the peak to fit
 */
static void fitbg(struct pf_data* d, int peak) {
	int i, status, iter = 0;
	double chi, ic, id;
	double bglbound, bgrbound;
	struct bg_poly_data pd;

	const gsl_multifit_fdfsolver_type *T;
	gsl_multifit_fdfsolver *s;

	gsl_matrix *covar;
	gsl_multifit_function_fdf f;

	const int pp = 4;

	double x1[pp];
	gsl_vector_view x;

	prepbgpts(d, peak, &pd, &bglbound, &bgrbound);

	covar = gsl_matrix_alloc(pp, pp);

	for (i = 0; i < pp; ++i) {
		x1[i] = 0;
	}

	/* prep the fit with a linear fit */

	prepbgfit(d, peak, &pd, &ic, &id);
	x1[pp-2] = ic;
	x1[pp-1] = id;

	x = gsl_vector_view_array(x1, pp);

	f.f = &polyb_f;
	f.df = &polyb_df;
	f.fdf = &polyb_fdf;
	f.n = pd.npts;
	f.p = pp;
	f.params = &pd;

	T = gsl_multifit_fdfsolver_lmsder;
	s = gsl_multifit_fdfsolver_alloc(T, pd.npts, pp);

	gsl_multifit_fdfsolver_set(s, &f, &x.vector);

	do {
		++iter;
		status = gsl_multifit_fdfsolver_iterate(s);

		if (status) {
			break;
		}

		status = gsl_multifit_test_delta(s->dx, s->x, 1e-4, 1e-4);
	} while (status == GSL_CONTINUE && iter < MAXITER);

	
	if (status == GSL_CONTINUE) {
		d->peaks[peak].bg.conv = 0;
	}
	else {
		d->peaks[peak].bg.conv = 1;
	}

	// gsl_multifit_covar(s->J, 0.0, covar);

	d->peaks[peak].bg.a = gsl_vector_get(s->x, 0);
	d->peaks[peak].bg.b = gsl_vector_get(s->x, 1);
	d->peaks[peak].bg.c = gsl_vector_get(s->x, 2);
	d->peaks[peak].bg.d = gsl_vector_get(s->x, 3);

	d->peaks[peak].bg.a_err = sqrt(gsl_matrix_get(covar,0,0));
	d->peaks[peak].bg.b_err = sqrt(gsl_matrix_get(covar,1,1));
	d->peaks[peak].bg.c_err = sqrt(gsl_matrix_get(covar,2,2));
	d->peaks[peak].bg.d_err = sqrt(gsl_matrix_get(covar,3,3));

	chi = gsl_blas_dnrm2(s->f);
	d->peaks[peak].bg.chisq = chi*chi;
	d->peaks[peak].bg.chisq_dof = (chi*chi)/(pd.npts - pp);

	d->peaks[peak].bglbound = bglbound;
	d->peaks[peak].bgrbound = bgrbound;

	gsl_multifit_fdfsolver_free(s);
	gsl_matrix_free(covar);
}

/**
 * Fit a given peak
 * @param[in,out] d The data set
 * @param[in] peak The peak to fit
 */
static void fitpeak(struct pf_data* d, int peak) {
	int i, pw, status, iter = 0;
	double chi, ia, ik, im, ix0;
	struct peak_pearson7_data pd;

	const gsl_multifit_fdfsolver_type *T;
	gsl_multifit_fdfsolver *s;

	gsl_matrix *covar;
	gsl_multifit_function_fdf f;

	const int pp = 4;

	double x1[pp];
	gsl_vector_view x;

	fitbg(d, peak);

	covar = gsl_matrix_alloc(pp, pp);

	for (i = 0; i < pp; ++i) {
		x1[i] = 0;
	}

	/* prep the fit with a linear fit */

	preppeakfit(d, peak, &ia, &ik, &ix0, &im);
	x1[0] = ia;
	x1[1] = ik;
	x1[2] = ix0;
	x1[3] = im;

	x = gsl_vector_view_array(x1, pp);

	pw = d->peaks[peak].rbin - d->peaks[peak].lbin + 1;

	pd.peak = peak;
	pd.d = d;

	f.f = &pearson7_f;
	f.df = &pearson7_df;
	f.fdf = &pearson7_fdf;
	f.n = pw;
	f.p = pp;
	f.params = &pd;

	T = gsl_multifit_fdfsolver_lmsder;
	s = gsl_multifit_fdfsolver_alloc(T, pw, pp);

	gsl_multifit_fdfsolver_set(s, &f, &x.vector);

	do {
		++iter;
		status = gsl_multifit_fdfsolver_iterate(s);

		if (status) {
			break;
		}

		status = gsl_multifit_test_delta(s->dx, s->x, 1e-4, 1e-4);
	} while (status == GSL_CONTINUE && iter < MAXITER);

	if (status == GSL_CONTINUE) {
		d->peaks[peak].fit.conv = 0;
	}
	else {
		d->peaks[peak].fit.conv = 1;
	}

	// gsl_multifit_covar(s->J, 0.0, covar);

	d->peaks[peak].fit.a = gsl_vector_get(s->x, 0);
	d->peaks[peak].fit.k = gsl_vector_get(s->x, 1);
	d->peaks[peak].fit.x0 = gsl_vector_get(s->x, 2);
	d->peaks[peak].fit.m = gsl_vector_get(s->x, 3);

	d->peaks[peak].fit.a_err = sqrt(gsl_matrix_get(covar,0,0));
	d->peaks[peak].fit.k_err = sqrt(gsl_matrix_get(covar,1,1));
	d->peaks[peak].fit.x0_err = sqrt(gsl_matrix_get(covar,2,2));
	d->peaks[peak].fit.m_err = sqrt(gsl_matrix_get(covar,3,3));

	chi = gsl_blas_dnrm2(s->f);
	d->peaks[peak].fit.chisq = chi*chi;
	d->peaks[peak].fit.chisq_dof = (chi*chi)/(pw - pp);

	gsl_multifit_fdfsolver_free(s);
	gsl_matrix_free(covar);

	d->peaks[peak].center = d->peaks[peak].fit.x0;
	d->peaks[peak].width = pearson7_fwhm(d->peaks[peak].fit.a, d->peaks[peak].fit.k, d->peaks[peak].fit.m);

	d->peaks[peak].height = d->data_raw[d->peaks[peak].cbin] - pf_bgfunc(d, peak, d->peaks[peak].cbin + 0.5);

	d->peaks[peak].num = peak;
}

/**
 * Determine if a floating point number is real
 * @param[in] n The number to test
 * @return Boolean flag indicating that the number is a real number
 */
static int isbadnum(double n) {
	if (isnan(n)) {
		return 1;
	}
	else if (isinf(n)) {
		return 1;
	}

	return 0;
}

/**
 * Determine if a peak fit is reasonable
 * @param[in] d The data set
 * @param[in] peak The index of the peak to test
 * @return Boolean flag indicating if the fit is reasonable
 */
static int reasonablefit(struct pf_data* d, int peak) {
	int pw;
	double pwb;

	if (isbadnum(d->peaks[peak].fit.a) || isbadnum(d->peaks[peak].fit.k) ||
		isbadnum(d->peaks[peak].fit.x0) || isbadnum(d->peaks[peak].fit.m)) {
		return 0;
	}

	/* the height cannot be negative */
	if (d->peaks[peak].fit.a < 0) {
		return 0;
	}

	/* a height of less than one count is not useful */
	if (d->peaks[peak].fit.a < 1.0) {
		return 0;
	}

	/* if more than 1.5 times the real height, this is not real */
	if (d->peaks[peak].fit.a > 1.5*d->data_raw[d->peaks[peak].cbin]) {
		return 0;
	}

	/* if less than 0.75 times the real height, this is not real */
	if (d->peaks[peak].fit.a < 0.75*(d->data_raw[d->peaks[peak].cbin] - pf_bgfunc(d, peak, d->peaks[peak].fit.x0))) {
		return 0;
	}

	/* the peak is too far to the left */
	if (d->peaks[peak].fit.x0 < d->peaks[peak].lbin) {
		return 0;
	}

	/* the peak is too far to the right */
	if (d->peaks[peak].fit.x0 > d->peaks[peak].rbin) {
		return 0;
	}

	/* the peak is more than 1.5 times wider than the identified interval */
	pw = d->peaks[peak].rbin - d->peaks[peak].lbin + 1;
	if (d->peaks[peak].width > 1.5*pw) {
		return 0;
	}

	/* if the peak is wider than the bound interval, then it is not good */
	pwb = d->peaks[peak].rbound - d->peaks[peak].lbound;
	if (d->peaks[peak].width > pwb) {
		return 0;
	}

	/* the width at fwhm cannot be less than 1 */
	if (d->peaks[peak].width < 1.0) {
		return 0;
	}
	
	return 1;
}

/**
 * Determine to bounds of a given peak based on the fit
 * @param[in,out] d The data set
 * @param[in] peak The index of the peak for which to determine bounds
 */
static void peakbounds(struct pf_data* d, int peak) {
	double pw, x, pwm;

	pwm = 0.75*BGWIDTH;
	pw = d->peaks[peak].rbin - d->peaks[peak].lbin + 1;
	for (x = d->peaks[peak].fit.x0; x > d->peaks[peak].fit.x0 - pwm*pw; x -= SEARCHSTEP) {
		if (fabs(pf_fitfunc(d, peak, x) - pf_bgfunc(d, peak, x)) < 1.0) {
			break;
		}
	}

	d->peaks[peak].lbound = x;

	for (x = d->peaks[peak].fit.x0; x < d->peaks[peak].fit.x0 + pwm*pw; x += SEARCHSTEP) {
		if (fabs(pf_fitfunc(d, peak, x) - pf_bgfunc(d, peak, x)) < 1.0) {
			break;
		}
	}

	d->peaks[peak].rbound = x;
}

/**
 * Calculate the area of a given peak
 * @param[in,out] d The data set
 * @param[in] peak The peak of which to calculate the area
 */
static void peakarea(struct pf_data* d, int peak) {
	int i, is;
	double tot;

	/* add up all counts between the bounds, interpolate fractionally */

	i = (int) floor(d->peaks[peak].lbound);
	is = (int) floor(d->peaks[peak].rbound);

	if (is >= NCHAN) {
		is = NCHAN - 1;
	}
	
	tot = (1.0 - (d->peaks[peak].lbound - i)) * (d->data_raw[i] - pf_bgfunc(d, peak, i + 0.5));
	for (; i < NCHAN && i < is; ++i) {
		tot += d->data_raw[i] - pf_bgfunc(d, peak, i + 0.5);
	}

	tot += (d->peaks[peak].rbound - is) * (d->data_raw[is] - pf_bgfunc(d, peak, is + 0.5));

	d->peaks[peak].area = tot;
}

/**
 * qsort compar function to sort pf_peak pointers by height
 * @param[in] ptr1 The first pointer
 * @param[in] ptr2 The second pointer
 * @return The difference between the height of the first peak and the height of the second peak
 */
static int comppeakheight(const void* ptr1, const void* ptr2) {
	const struct pf_peak* p1;
	const struct pf_peak* p2;

	p1 = *((const struct pf_peak**) ptr1);
	p2 = *((const struct pf_peak**) ptr2);

	return (int) (p2->height - p1->height);
}

/**
 * qsort compar function to sort pf_peak pointers by area
 * @param[in] ptr1 The first pointer
 * @param[in] ptr2 The second pointer
 * @return The difference between the area of the first peak and the area of the second peak
 */
static int comppeakarea(const void* ptr1, const void* ptr2) {
	const struct pf_peak* p1;
	const struct pf_peak* p2;

	p1 = *((const struct pf_peak**) ptr1);
	p2 = *((const struct pf_peak**) ptr2);

	return (int) (p2->area - p1->area);
}

/**
 * Sort the peaks by height and area
 * @param[in,out] d The data set
 */
static void sortpeaks(struct pf_data* d) {
	int i;

	for (i = 0; i < d->npeaks; ++i) {
		d->peaks_by_number[i] = &d->peaks[i];
		d->peaks_by_height[i] = &d->peaks[i];
		d->peaks_by_area[i] = &d->peaks[i];
	}

	qsort(d->peaks_by_height, d->npeaks, sizeof(struct pf_peak*), comppeakheight);
	qsort(d->peaks_by_area, d->npeaks, sizeof(struct pf_peak*), comppeakarea);
}

void pf_prunepeaks(struct pf_data* d) {
	int i, npeaks;
	struct pf_peak peaks[NCHAN];

	npeaks = 0;
	for (i = 0; i < d->npeaks; ++i) {
		if (reasonablefit(d, i)) {
			peaks[npeaks++] = d->peaks[i];
		}
	}

	for (i = 0; i < npeaks; ++i) {
		d->peaks[i] = peaks[i];
		d->peaks[i].num = i;
	}

	d->npeaks = npeaks;

	sortpeaks(d);
}

void pf_fitpeaks(struct pf_data* d) {
	int i;

	for (i = 0; i < d->npeaks; ++i) {
		fitpeak(d, i);
		peakbounds(d, i);
		peakarea(d, i);
	}
}

double pf_bgfunc(struct pf_data* d, int peak, double x) {
	return polyb(d->peaks[peak].bg.a, d->peaks[peak].bg.b, d->peaks[peak].bg.c, d->peaks[peak].bg.d, x);
}

double pf_peakfunc(struct pf_data* d, int peak, double x) {
	return pearson7(d->peaks[peak].fit.a, d->peaks[peak].fit.k, d->peaks[peak].fit.x0, d->peaks[peak].fit.m, x);
}

double pf_fitfunc(struct pf_data* d, int peak, double x) {
	return pf_peakfunc(d, peak, x) + pf_bgfunc(d, peak, x);
}

