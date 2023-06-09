/**
  @file     gnuplot_i.h
  @author   N. Devillard
  @date     Sep 1998
  @version  $Revision: 1.11 $
  @brief    C interface to gnuplot.

  gnuplot is a freely available, command-driven graphical display tool for
  Unix. It compiles and works quite well on a number of Unix flavours as
  well as other operating systems. The following module enables sending
  display requests to gnuplot through simple C calls.

*/

#ifndef ENSEN_UI_GNUPLOT_H
#define ENSEN_UI_GNUPLOT_H

#include <stdio.h>

/** Maximal number of simultaneous temporary files */
#define GP_MAX_TMP_FILES    64

/**
  @typedef  gnuplot_ctrl
  @brief    gnuplot session handle (opaque type).

  This structure holds all necessary information to talk to a gnuplot
  session. It is built and returned by gnuplot_init() and later used
  by all functions in this module to communicate with the session, then
  meant to be closed by gnuplot_close().

  This structure is meant to remain opaque, you normally do not need
  to know what is contained in there.
 */
typedef struct _GNUPLOT_CTRL_ {
    /** Pipe to gnuplot process */
    FILE    * gnucmd ;

    /** Number of currently active plots */
    int       nplots ;
    /** Current plotting style */
    char      pstyle[32] ;

    /** Pointer to table of names of temporary files */
    char*      tmp_filename_tbl[GP_MAX_TMP_FILES] ;
    /** Number of temporary files */
    int       ntmp ;
} gnuplot_ctrl ;

/**
  @brief    Opens up a gnuplot session, ready to receive commands.
  @return   Newly allocated gnuplot control structure.

  This opens up a new gnuplot session, ready for input. The struct
  controlling a gnuplot session should remain opaque and only be
  accessed through the provided functions.

  The session must be closed using gnuplot_close().
 */
gnuplot_ctrl * gnuplot_init(void);

/**
  @brief    Closes a gnuplot session previously opened by gnuplot_init()
  @param    handle Gnuplot session control handle.
  @return   void

  Kills the child PID and deletes all opened temporary files.
  It is mandatory to call this function to close the handle, otherwise
  temporary files are not cleaned and child process might survive.

 */
void gnuplot_close(gnuplot_ctrl * handle);

/**
  @brief    Sends a command to an active gnuplot session.
  @param    handle Gnuplot session control handle
  @param    cmd    Command to send, same as a printf statement.

  This sends a string to an active gnuplot session, to be executed.
  There is strictly no way to know if the command has been
  successfully executed or not.
  The command syntax is the same as printf.

  Examples:

  @code
  gnuplot_cmd(g, "plot %d*x", 23.0);
  gnuplot_cmd(g, "plot %g * cos(%g * x)", 32.0, -3.0);
  @endcode

  Since the communication to the gnuplot process is run through
  a standard Unix pipe, it is only unidirectional. This means that
  it is not possible for this interface to query an error status
  back from gnuplot.
 */
void gnuplot_cmd(gnuplot_ctrl *  handle, char const *  cmd, ...);

/**
  @brief    Change the plotting style of a gnuplot session.
  @param    h Gnuplot session control handle
  @param    plot_style Plotting-style to use (character string)
  @return   void

  The provided plotting style is a character string. It must be one of
  the following:

  - lines
  - points
  - linespoints
  - impulses
  - dots
  - steps
  - errorbars
  - boxes
  - boxeserrorbars
 */
void gnuplot_setstyle(gnuplot_ctrl * h, char * plot_style);

/**
  @brief    Sets the x label of a gnuplot session.
  @param    h Gnuplot session control handle.
  @param    label Character string to use for X label.
  @return   void

  Sets the x label for a gnuplot session.
 */
void gnuplot_set_xlabel(gnuplot_ctrl * h, char * label);


/**
  @brief    Sets the y label of a gnuplot session.
  @param    h Gnuplot session control handle.
  @param    label Character string to use for Y label.
  @return   void

  Sets the y label for a gnuplot session.
 */
void gnuplot_set_ylabel(gnuplot_ctrl * h, char * label);

/**
  @brief    Resets a gnuplot session (next plot will erase previous ones).
  @param    h Gnuplot session control handle.
  @return   void

  Resets a gnuplot session, i.e. the next plot will erase all previous
  ones.
 */
void gnuplot_resetplot(gnuplot_ctrl * h);

/**
  @brief    Plots a 2d graph from a list of doubles.
  @param    handle  Gnuplot session control handle.
  @param    d       Array of doubles.
  @param    n       Number of values in the passed array.
  @param    title   Title of the plot.
  @return   void

  Plots out a 2d graph from a list of doubles. The x-coordinate is the
  index of the data_t in the list, the y coordinate is the data_t in
  the list.

  Example:

  @code
    gnuplot_ctrl    *h ;
    data_t          d[50] ;
    int             i ;

    h = gnuplot_init() ;
    for (i=0 ; i<50 ; i++) {
        d[i] = (data_t)(i*i) ;
    }
    gnuplot_plot_x(h, d, 50, "parabola") ;
    sleep(2) ;
    gnuplot_close(h) ;
  @endcode
 */
void gnuplot_plot_x(gnuplot_ctrl * handle, data_t * d, int n, char * title);

/**
  @brief    Plot a 2d graph from a list of points.
  @param    handle      Gnuplot session control handle.
  @param    x           Pointer to a list of x coordinates.
  @param    y           Pointer to a list of y coordinates.
  @param    n           Number of doubles in x (assumed the same as in y).
  @param    title       Title of the plot.
  @return   void

  Plots out a 2d graph from a list of points. Provide points through a list
  of x and a list of y coordinates. Both provided arrays are assumed to
  contain the same number of values.

  @code
    gnuplot_ctrl    *h ;
    data_t          x[50] ;
    data_t          y[50] ;
    int             i ;

    h = gnuplot_init() ;
    for (i=0 ; i<50 ; i++) {
        x[i] = (data_t)(i)/10.0 ;
        y[i] = x[i] * x[i] ;
    }
    gnuplot_plot_xy(h, x, y, 50, "parabola") ;
    sleep(2) ;
    gnuplot_close(h) ;
  @endcode
 */
void gnuplot_plot_xy(
    gnuplot_ctrl    *   handle,
    data_t          *   x,
    data_t          *   y,
    int                 n,
    char            *   title
) ;

// void gnuplot_plot_points(
//     gnuplot_ctrl     *handle,
//     Point           (*pnt)[],
//     int               n,
//     char             *title
// );

void gnuplot_plot_points(
    gnuplot_ctrl     *handle,
    Points           *points,
    int               n,
    char             *title
);

void
gnuplot_plot_double(
    gnuplot_ctrl     *handle,
    Points           *data_points,
    data_t           *data,
    int               n,
    char             *title
);

/**
  @brief    Open a new session, plot a signal, close the session.
  @param    title   Plot title
  @param    style   Plot style
  @param    label_x Label for X
  @param    label_y Label for Y
  @param    x       Array of X coordinates
  @param    y       Array of Y coordinates (can be NULL)
  @param    n       Number of values in x and y.
  @return

  This function opens a new gnuplot session, plots the provided
  signal as an X or XY signal depending on a provided y, waits for
  a carriage return on stdin and closes the session.

  It is Ok to provide an empty title, empty style, or empty labels for
  X and Y. Defaults are provided in this case.
 */
void gnuplot_plot_once(
    char    *   title,
    char    *   style,
    char    *   label_x,
    char    *   label_y,
    data_t  *   x,
    data_t  *   y,
    int         n
);

/**
  @brief    Plot a slope on a gnuplot session.
  @param    handle      Gnuplot session control handle.
  @param    a           Slope.
  @param    b           Intercept.
  @param    title       Title of the plot.
  @return   void

  Plot a slope on a gnuplot session. The provided slope has an
  equation of the form y=ax+b

  Example:

  @code
    gnuplot_ctrl    *   h ;
    data_t              a, b ;

    h = gnuplot_init() ;
    gnuplot_plot_slope(h, 1.0, 0.0, "unity slope") ;
    sleep(2) ;
    gnuplot_close(h) ;
  @endcode
 */
void gnuplot_plot_slope(
    gnuplot_ctrl    *   handle,
    data_t              a,
    data_t              b,
    char            *   title
) ;

/**
  @brief    Plot a curve of given equation y=f(x).
  @param    h           Gnuplot session control handle.
  @param    equation    Equation to plot.
  @param    title       Title of the plot.
  @return   void

  Plots out a curve of given equation. The general form of the
  equation is y=f(x), you only provide the f(x) side of the equation.

  Example:

  @code
        gnuplot_ctrl    *h ;
        char            eq[80] ;

        h = gnuplot_init() ;
        strcpy(eq, "sin(x) * cos(2*x)") ;
        gnuplot_plot_equation(h, eq, "sine wave", normal) ;
        gnuplot_close(h) ;
  @endcode
 */
void gnuplot_plot_equation(gnuplot_ctrl * h, char * equation, char * title) ;

/**
 * Writes a CSV file for use with gnuplot commands later.  Allows files to also be saved for post
 * analysis with excel for example. Arguments are similar to gnuplot_plot_x()
 *
 * Uses title as gnuplot "comment" on the first line.
 *
 * @author Peter H Maresh 11/8/2011
 *
 * @param fileName file name to write to.  This should be the same name used in the gnuplot command
 * @param d
 * @param n
 * @param title
 * @return int      <0 if error writing file.
 *
 */
int gnuplot_write_x_csv(
    char const * fileName,
    data_t const * d,
    int n,
    char const * title);

/**
 * Writes a CSV file for use with gnuplot commands later.  Allows files to also be saved for post
 * analysis with excel for example. Arguments are similar to gnuplot_plot_xy()
 *
 * Uses title as gnuplot "comment" on the first line.
 *
 * @author Peter H Maresh 11/8/2011
 *
 * @param fileName file name to write to.  This should be the same name used in the gnuplot command
 * @param x
 * @param y
 * @param n
 * @param title
 * @return int <0 if file wasn't written.
 */
int gnuplot_write_xy_csv(
    char const *        fileName,
    data_t const    *   x,
    data_t const    *   y,
    int                 n,
    char const      *   title);

/**
 * Writes a multi column CSV file for use with gnuplot commands later.  Allows files to also be
 * saved for post analysis with excel for example. Note that when used with gnuplot, since there
 * may be more than one column the whole "1:3" or whatever should be used.
 *
 * Uses title as gnuplot "comment" on the first line.
 *
 * @author Peter H Maresh 11/8/2011
 *
 * @param fileName      file name to write to.  This should be the same name used in the gnuplot
 *                      command
 * @param xListPtr      A list of pointers to column buffers
 * @param n
 * @param numColumns    Length of xListPtr list.  Note that the CSV file will add an additional
 *                      "index" column first.
 * @param title         Title to write for the first line of the .csv file, will be preceeded by
 *                      "#"
 * @return int <0 if file wasn't written.
 */
int gnuplot_write_multi_csv(
    char const *        fileName,
    data_t const    **  xListPtr,
    int                 n,
    int                 numColumns,
    char const      *   title);

#endif
