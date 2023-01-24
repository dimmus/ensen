#include <stdio.h>
#include <math.h>

#include <signal_fit.h>
#include <signal_data.h>

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
//   printf("The polynomial fit is given by the equation:\n");
//   signal_fit_printMatrix(n_poly+1,n_poly+2,B);
  signal_fit_gaussEliminationLS(n_poly+1,n_poly+2,B,A);
//   for(i = 0; i <= n_poly; i++){
//       printf("%lfx^%d+",A[i],i);
//   }

  for (i = 0; i < n_points; i++)
  {
    (*p)[i].x = 0;
    for(j = 0; i <= n_poly; i++)
    {
      (*p)[i].x += A[j]*pow(y[i], j);
    }
  }
}

/**
    @brief Fast smooth Y with defined smooth width
    @param p Pointer to the array of points (x and y)
    @param n_points Total number of points
    @param smoothwidth Width of the smooth window (number of points)
**/
void
fsmooth(Point (*p)[], index_t n_points, float smoothwidth)
{
    index_t w = round(smoothwidth);
    double SumPoints = 0.0;
    for (index_t i = 0; i < w; i++) SumPoints += (*p)[i].y;
    double s[n_points];
    index_t halfw = round(w/2);
    for (index_t k = 0; k <= (n_points-w); k++)
    {
        s[k + halfw - 1] = SumPoints;
        SumPoints = SumPoints - (*p)[k].y;
        SumPoints = SumPoints + (*p)[k + w].y;
    }
    for (index_t n = n_points - w + 1; n <= n_points; n++)
    {
        s[n_points - w + halfw] += (*p)[n].y;
    }
    for (index_t i = 0; i < n_points; i++)
    {
        (*p)[i].y = s[i]/w;
    }
}


index_t
val2int(double x[], index_t n_points, double val)
{
    double diff[n_points];
    index_t i, index = 0;
    
    for (i = 0; i < n_points; i++)
    {
        diff[i] = abs(x[i] - val);
    }
    
    double min_val = min(diff, n_points);
    
    for (i = 0; i < n_points; i++)
    {
        if (diff[i]-min_val == 0)
        {
            index = i;
        }
        
    }
    
    return index;
}

/// @brief Find minimal value in array
/// @param x Array
/// @param n_points Number of points in array (array size)
/// @return Minimal value in array
double
min(double x[], index_t n_points)
{
    double min = x[0];
    for (index_t i = 0; i < n_points; i++)
    {
        if (min > x[i]) min = x[i];
    }
    return min;
}