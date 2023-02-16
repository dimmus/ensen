#ifndef ENSEN_SIGNAL_FIT_H
#define ENSEN_SIGNAL_FIT_H

void signal_fit_gaussEliminationLS(int m, int n, double a[m][n], double x[n-1]);
void signal_fit_printMatrix(int m, int n, double matrix[m][n]);
void signal_fit(Point (*p)[], index_t n_points, index_t n_poly);
void smooth(data_t *y, index_t n_points, data_t smoothwidth);
index_t val2ind(data_t *x, index_t n_points, data_t val);
data_t min(data_t *x, index_t n_points);
data_t max(data_t *x, index_t n_points);
data_t sum(data_t *x, index_t n_points);
void data_window_get(Point (*p)[], index_t n_points, data_t center, data_t window, Point (*segment)[]);
void deriv(index_t size, data_t * in, data_t * out);
void deriv_points(index_t size, Points * in, data_t * out);
index_t findpeak(index_t size, data_t * input);
data_t findpeaks(data_t * input, Peaks * p, Signal_Parameters * conf);

#endif
