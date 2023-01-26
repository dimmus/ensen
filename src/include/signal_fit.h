#ifndef SIGNAL_FIT_H
#define SIGNAL_FIT_H

#include <signal_data.h>

void signal_fit_gaussEliminationLS(int m, int n, double a[m][n], double x[n-1]);
void signal_fit_printMatrix(int m, int n, double matrix[m][n]);
void signal_fit(Point (*p)[], index_t n_points, index_t n_poly);
void fsmooth(Point (*p)[], index_t n_points, float smoothwidth);
index_t val2int(double x[], index_t n_points, double val);
double min(double x[], index_t n_points);

#endif
