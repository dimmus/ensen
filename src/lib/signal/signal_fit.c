#include <stdio.h>
#include <math.h>

#include "ensen_private.h"
#include "ensen_benchmark.h"
#include "ensen_signal_fit.h"

/**
    Function that performs Gauss-Elimination and returns the Upper triangular matrix and solution of equations:
    There are two options to do this in C.
    1. Pass the augmented matrix (a) as the parameter, and calculate and store the upperTriangular(Gauss-Eliminated Matrix) in it.
    2. Use malloc and make the function of pointer type and return the pointer.
    This program uses the first option.
**/
void 
signal_fit_gaussEliminationLS(int m, int n, double a[m][n], double x[n-1])
{
    int i,j,k;
    for(i=0;i<m-1;i++){
        //Partial Pivoting
        for(k=i+1;k<m;k++){
            //If diagonal element(absolute vallue) is smaller than any of the terms below it
            if(fabs(a[i][i])<fabs(a[k][i])){
                //Swap the rows
                for(j=0;j<n;j++){                
                    double temp;
                    temp=a[i][j];
                    a[i][j]=a[k][j];
                    a[k][j]=temp;
                }
            }
        }
        //Begin Gauss Elimination
        for(k=i+1;k<m;k++){
            double  term=a[k][i]/ a[i][i];
            for(j=0;j<n;j++){
                a[k][j]=a[k][j]-term*a[i][j];
            }
        }
         
    }
    //Begin Back-substitution
    for(i=m-1;i>=0;i--){
        x[i]=a[i][n-1];
        for(j=i+1;j<n-1;j++){
            x[i]=x[i]-a[i][j]*x[j];
        }
        x[i]=x[i]/a[i][i];
    }
             
}

/**
    Function that prints the elements of a matrix row-wise
    Parameters: rows(m),columns(n),matrix[m][n] 
**/
void 
signal_fit_printMatrix(int m, int n, double matrix[m][n])
{
    int i,j;
    for(i=0;i<m;i++){
        for(j=0;j<n;j++){
            printf("%lf\t",matrix[i][j]);
        }
        printf("\n");
    } 
}

/**
    @brief Polynomial fit
    @param p Points of data to fit
    @param n_points Total number of points
    @param n_poly Polynom order
**/
void
signal_fit(Point (*p)[], index_t n_points, index_t n_poly)
{
  //arrays to store the c and y-axis data-points
  index_t i,j; 
  double x[n_points];
  double y[n_points];
  for (i = 0; i < n_points; i++)
  {
    x[i] = (*p)[i].x;
    y[i] = (*p)[i].y;
  }
  // an array of size 2*n_poly+1 for storing n_points, Sig xi, Sig xi^2, ...., etc. which are the independent components of the normal matrix
  double X[2*n_poly+1]; 
  for(i=0;i<=2*n_poly;i++){
      X[i]=0;
      for(j=0;j<n_points;j++){
          X[i]=X[i]+pow(x[j],i);
      }
  }
  //the normal augmented matrix
  double B[n_poly+1][n_poly+2];  
  // rhs
  double Y[n_poly+1];      
  for(i=0;i<=n_poly;i++){
      Y[i]=0;
      for(j=0;j<n_points;j++){
          Y[i]=Y[i]+pow(x[j],i)*y[j];
      }
  }
  for(i=0;i<=n_poly;i++){
      for(j=0;j<=n_poly;j++){
          B[i][j]=X[i+j]; 
      }
  }
  for(i=0;i<=n_poly;i++){
      B[i][n_poly+1]=Y[i];
  }
  double A[n_poly+1];
  printf("The polynomial fit is given by the equation:\n");
  signal_fit_printMatrix(n_poly+1,n_poly+2,B);
  signal_fit_gaussEliminationLS(n_poly+1,n_poly+2,B,A);
  for(i = 0; i <= n_poly; i++){
      printf("%lfx^%d+",A[i],i);
  }

  for (i = 0; i < n_points; i++)
  {
    (*p)[i].y = 0;
    for(j = 0; i <= n_poly; i++)
    {
      (*p)[i].y += A[j]*pow(y[i], j);
    }
  }
}

/**
    @brief Fast smooth Y with defined smooth width
    @param p Pointer to the array of points (x and y)
    @param n_points Total number of points
    @param smoothwidth Width of the smooth window (number of points)
**/
// void
// fsmooth(Point (*p)[], index_t n_points, float smoothwidth)
// {
//     index_t w = round(smoothwidth);
    
//     data_t SumPoints = 0.0;
//     for (index_t i = 0; i < w; i++)
//     {
//         SumPoints += (*p)[i].y;
//     }
    
//     data_t s[n_points];
//     index_t halfw = round(w/2);
//     for (index_t k = 0; k <= (n_points-w); k++)
//     {
//         s[k + halfw - 1] = SumPoints;
//         SumPoints -= (*p)[k].y;
//         SumPoints += (*p)[k + w].y;
//     }

//     for (index_t n = n_points - w + 1; n <= n_points; n++)
//     {
//         s[n_points - w + halfw] += (*p)[n].y;
//     }

//     for (index_t i = 0; i < n_points; i++)
//     {
//         (*p)[i].y = s[i]/w;
//     }
// }

void
smooth(data_t *y, index_t n_points, index_t smoothwidth)
{
    index_t w = smoothwidth;
    
    data_t SumPoints = 0.0;
    for (index_t i = 0; i < w; i++)
    {
        /* hack to avoid appearing of 'nan' in data */
        if(y[i] >= 0 || y[i] < 0) SumPoints += y[i];
    }
    
    data_t s[n_points];
    index_t halfw = round(w/2);
    for (index_t k = 0; k <= (n_points-w); k++)
    {
        s[k + halfw - 1] = SumPoints;
        if(y[k] >= 0 || y[k] < 0) SumPoints -= y[k]; 
        if(y[k + w] >= 0 || y[k + w] < 0) SumPoints += y[k + w];
    }

    for (index_t n = n_points - w + 1; n <= n_points; n++)
    {
        if(y[n] >= 0 || y[n] < 0) s[n_points - w + halfw] += y[n];
    }

    for (index_t i = 0; i < n_points; i++)
    {
        y[i] = s[i]/w;
    }
}

index_t
val2ind(data_t *x, index_t n_points, data_t val)
{
    data_t diff[n_points];
    index_t i, index = 0;
    
    for (i = 0; i < n_points; i++)
    {
        diff[i] = x[i] - val;
        if (diff[i] < 0) diff[i] = diff[i] * (-1);
    }
    
    data_t min_val = min(diff, n_points);
    
    for (i = 0; i < n_points; i++)
    {
        if (fabs(diff[i] - min_val) <= 1.0e-14) index = i; /* solved warning: comparing floating-point with ‘==’ or ‘!=’ is unsafe */
    }
    
    return index;
}

/// @brief Find minimal value in array
/// @param x Array
/// @param n_points Number of points in array (array size)
/// @return Minimal value in array
data_t
min(data_t *x, index_t n_points)
{
    data_t min = x[0];
    for (index_t i = 0; i < n_points; i++)
    {
        if (min > x[i]) min = x[i];
    }
    return min;
}

/// @brief Find minimal value in array (unsigned or absolute)
/// @param x Array
/// @param n_points Number of points in array (array size)
/// @return Minimal absulute value in array
data_t
min_abs(data_t *x, index_t n_points)
{
    data_t min = x[1];
    if (min < 0) { min = min * (-1); }    
    for (index_t i = 1; i < n_points; i++)
    {
        if (x[i] < 0) x[i] = x[i] * (-1);
        if (min > x[i]) min = x[i];
    }
    return min;
}

/// @brief Find maximum value in array
/// @param x Array
/// @param n_points Number of points in array (array size)
/// @return Maximum value in array
data_t
max(data_t *x, index_t n_points)
{
    data_t max = x[0];
    for (index_t i = 0; i < n_points; i++)
    {
        if (max < x[i]) max = x[i];
    }
    return max;
}

/// @brief Find maximum value in array (unsigned or absolute)
/// @param x Array
/// @param n_points Number of points in array (array size)
/// @return Maximum absolute value in array
data_t
max_abs(data_t *x, index_t n_points)
{
    data_t max = x[0];
    for (index_t i = 0; i < n_points; i++)
    {
        if (x[i] < 0) x[i] = x[i] * (-1);
        if (max < x[i]) max = x[i];
    }
    return max;
}

/// @brief Find sum of all values in array
/// @param x Array
/// @param n_points Number of points in array (array size)
/// @return Find sum of all values in array
data_t
sum(data_t *x, index_t n_points)
{
    data_t sum = x[0];
    for (index_t i = 0; i < n_points; i++)
    {
        sum += x[i];
    }
    return sum;
}


/**
    @brief Isolate desired dataset segment (window) for curvefiting
    @param p Pointer to the array of points (x and y)
    @param n_points Total number of points
    @param smoothwidth Width of the smooth window (number of points)
**/
void
data_window_get(Point (*p)[], index_t n_points, data_t center, data_t window_size, Point (*segment)[])
{
    data_t xoffset = 0.0;
    index_t n1 = 0, n2 = 0, n = 0, i = 0;
    data_t xx[n_points], yy[n_points];

    for (n = 0; n < n_points; n++)
    {
        xx[n] = (*p)[n].x;
        yy[n] = (*p)[n].y;
    }
    
    if (fabs(center) <= 1.0e-14 || fabs(window_size) <= 1.0e-14) /* solved warning: comparing floating-point with ‘==’ or ‘!=’ is unsafe */
    {
        window_size = max(xx, n_points) - min(xx, n_points);
        center = window_size/2;
    }

    n1 = val2ind(xx, n_points, center - (window_size / 2));
    n2 = val2ind(xx, n_points, center + (window_size / 2));

    if (fabs(window_size) <= 1.0e-14) /* solved warning: comparing floating-point with ‘==’ or ‘!=’ is unsafe */
    {
        n1 = 1; n2 = n_points;
    }

    index_t s = n1;
    for (i = 0; i < (n2 - n1); i++)
    {
        (*segment)[i].x = xx[s] - xoffset;
        (*segment)[i].y = yy[s];
        s = s + 1;
    }
}

/// @brief First derivative of vector using 2-point central difference.
/// @param size Size of input and output data array
/// @param in Input data array
/// @param out Output data array
void
deriv(index_t size, data_t * in, data_t * out)
{
    out[1] = in[2] - in[1];
    out[size] = in[size] - in[size - 1];
    
    for (index_t i = 2; i < size - 1; i++)
    {
        out[i] = (in[i + 1] - in[i - 1])/2;
    }
}

void
deriv_points(index_t size, Points * in, data_t * out)
{
    out[1] = (*in).y[2] - (*in).y[1];
    out[size] = (*in).y[size] - (*in).y[size - 1];
    
    for (index_t i = 2; i < size - 1; i++)
    {
        out[i] = ((*in).y[i + 1] - (*in).y[i - 1])/2;
    }
}

index_t
findpeak(index_t size, data_t * input)
{
    data_t d[size];
    deriv(size, input, d);
    index_t peak_pos = 0;

    for (index_t i = 0; i < size-1; i++)
    {
        if ((d[i] >= 0) & (d[i + 1] < 0)) // Detects zero-crossing of derivative
        {
            peak_pos = i + 1;
        }
    }
    return peak_pos;
}

data_t
findpeaks(data_t * y, Peaks * p, Signal_Parameters * conf)
{
    double start_time = get_run_time();
    data_t dy[(*conf).n_points];

    deriv((*conf).n_points, y, dy);
    // smooth(dy, size, 4);
    
    index_t num_of_peaks = 0;
    data_t diff = 0.f;
    (*p).total_number = 0;
    // printf("=====================\n");
    for (index_t i = 0; i < ((*conf).n_points - 1); i++)
    {
        if ((dy[i] >= 0) & (dy[i + 1] < 0) & (dy[i] <= 1)) // Detects zero-crossing of derivative
        {
            // printf("_Detected zero-crossing at [%d] with values [%f - %f]\n", i, dy[i], dy[i + 1]);
            if (y[i] > (*conf).search.threshold_amp) // Try to find peak above certain value of amplitude
            {
                // printf("___Detected peak at [%d] with amp [%f] (threshold: [%f]) \n", i, y[i], amp_threshold);
                // printf("___slope at [%d] with value [%f] (threshold: [%f])\n", i, dy[i] - dy[i + 1], slope_threshold);                    
                if ((dy[i] - dy[i + 1]) > (*conf).search.threshold_slope) // Try to find real peak (not fluctuations)
                {
                    // printf("______Detected slope at [%d] with value [%f] (threshold: [%f])\n", i, dy[i] - dy[i + 1], slope_threshold);
                    if (num_of_peaks >= (*conf).search.peaks_array_number) // out of peaks array size
                    {
                        printf("Warning: Found too many peaks. Out of array size. \n");
                        double end_time = get_run_time();
                        return end_time - start_time;
                    }
                    
                    (*p).peak[num_of_peaks].position = i;
                    ++num_of_peaks;
                    (*p).total_number = num_of_peaks;
                    
                    /* 
                     * avoid peak duplication
                     * if we find peak close to the existing one - 
                     * save only the previous (set number of peaks minus 1) 
                    */
                    if (num_of_peaks != 1) diff = (data_t)((*p).peak[num_of_peaks-1].position-(*p).peak[num_of_peaks-2].position)/(*conf).n_points;
                    if ((num_of_peaks != 1) & (diff < 0.05)) // diff threshold
                    {
                        (*p).peak[num_of_peaks-1].position = (*p).peak[num_of_peaks-2].position;
                        num_of_peaks = num_of_peaks - 1;
                        (*p).total_number = num_of_peaks;
                    }
                }
            }
        }
    }
    double end_time = get_run_time();
    return end_time - start_time;
}
