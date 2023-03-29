/**
 * @file plotdata.c
 * @author Hal Finkel
 *
 * Data and peak plotting
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <gsl/gsl_statistics.h>

#include <plplot/plplot.h>

#include "ensen_private.h"
#include "signal_peaksearch_commands.h"
#include "signal_peaksearch_common.h"

/**
 * The maximum length of the plot title
 */
#define MAXTITLE 256

/**
 * The number of points to draw per channel
 */
#define CHANPTS 10

/**
 * The default plplot output device
 */
#define DEFDEV "xwin"

/**
 * An association between a file-name extension and a plotting device name
 */
struct def_file_dev {
	char* ext; /**< The file-name extension */
	char* dev; /**< The plotting device name */
};

/**
 * Mapping of file-name extensions to plotting device names
 */
const struct def_file_dev def_file_devs[] = {
	{ "plmeta", "plmeta" },
	{ "plm", "plmeta" },
	{ "ps", "ps" },
	{ "psc", "psc" },
	{ "xfig", "xfig" },
	{ "imp", "imp" },
	{ "pbm", "pbm" },
	{ "jpg", "jpeg" },
	{ "jpeg", "jpeg" },
	{ "png", "png" },
	{ "cgm", "cgm" }
};

/**
 * Number of entries in the def_file_devs array
 */
const int def_file_devsn = sizeof(def_file_devs)/sizeof(def_file_devs[0]);

/**
 * Compare two string in a case-insensitive manner
 * @param[in] s1 The first string
 * @param[in] s2 The second string
 * @return Boolean equalivance indicator
 */
static int cmpext(char* s1, char* s2) {
	while (*s1 != '\0' && tolower(*s1) == tolower(*s2)) {
		++s1;
		++s2;
	}

	if (tolower(*s1) == tolower(*s2)) {
		return 1;
	}
	else {
		return 0;
	}
}

/**
 * Determine the output plotting device
 * @param[in] fn The output file name
 * @return The device name
 */
static char* finddev(char* fn) {
	int i;
	char* ext;

	if (!fn) {
		return DEFDEV;
	}

	ext = strrchr(fn, '.');
	if (!ext) {
		return DEFDEV;
	}

	++ext;

	if (*ext == '\0') {
		return DEFDEV;
	}

	for (i = 0; i < def_file_devsn; ++i) {
		if (cmpext(ext, def_file_devs[i].ext)) {
			return def_file_devs[i].dev;
		}
	}

	return DEFDEV;
}

/**
 * Annotate the graph with peak numbers
 * @param[in] d The data set
 * @param[in] od The values to plot
 * @param[in] p The plotting parameters
 * @param[in] cal The calibration fit
 */
static void annotgraph(struct pf_data* d, double* od, struct pf_plot_params* p, struct pf_cal_fit* cal) {
	int i;
	char pt[MAXTITLE];

	plcol0(15);
	plschr(0.0, 0.75);

	for (i = 0; i < d->npeaks; ++i) {
		double x, y;

		snprintf(pt, MAXTITLE, "%d", i);

		if (p->cal && cal->valid) {
			x = pf_calfunc(cal, d->peaks[i].cbin);
		}
		else {
			x = d->peaks[i].cbin;
		}

		y = od[d->peaks[i].cbin];
		plptex(x, y, 100, 2.5, 0.0, pt);
	}
}

/**
 * Mark the graph to indicate non-background regions
 * @param[in] d The data set
 * @param[in] od The values to plot
 * @param[in] p The plotting parameters
 * @param[in] cal The calibration fit
 */
static void markgraph(struct pf_data* d, double* od, struct pf_plot_params* p, struct pf_cal_fit* cal) {
	int i;

	plcol0(7);

	for (i = 0; i < NCHAN; ++i) {
		if (!d->data_bg[i]) {
			double vertx[4];
			double verty[4];

			if (p->cal && cal->valid) {
				vertx[0] = vertx[1] = pf_calfunc(cal, i);
				vertx[2] = vertx[3] = pf_calfunc(cal, i+1);
			}
			else {
				vertx[0] = vertx[1] = i;
				vertx[2] = vertx[3] = i+1;
			}

			verty[0] = verty[3] = 0;
			verty[1] = od[i];
			if (i < NCHAN - 1) {
				verty[2] = od[i+1];
			}
			else {
				verty[2] = od[i];
			}

			plfill(4, vertx, verty);
		}
	}
}

/**
 * Initialze plotting
 * @param[in] fn Output file or X11 terminal name
 * @param[in] dev The plotting device name
 * @param[in] rot The number of degrees to rotate the plot
 */
static void initgraph(char* fn, char* dev, double rot) {
	if (!dev) {
		dev = finddev(fn);
	}

	/* set plot to be black on white */
	plscol0(0, 0xFF, 0xFF, 0xFF);
	plscol0(1, 0x00, 0x00, 0x00);
	plscol0(15, 0xFF, 0x00, 0x00);

	plsetopt("plwindow", "peakfinder");

	if (fn) {
		plsfnam(fn);
	}

	plsdiori(rot/90.0);

	plsdev(dev);
	plinit();
}

/**
 * Calculate the domain in channels if specified in calibrated units
 * @param[in] d The data set
 * @param[in,out] p The plot parameters
 * @param[in] cal The calibration fit
 * @return Boolean success indicator
 */
int finddomain(struct pf_data* d ENSEN_UNUSED, struct pf_plot_params* p, struct pf_cal_fit* cal) {
	int i;

	if ((p->xstartcal || p->xendcal) && !cal->valid) {
		printf("cannot use calibrated domain without calibration\n");
		return 0;
	}

	if (p->xstartcal) {
		for (i = 0; i < NCHAN && pf_calfunc(cal, i) < p->xstart; ++i);
		p->xstart = i;
		p->xstartcal = 0;
	}
	if (p->xendcal) {
		for (i = NCHAN - 1; i >= 0 && pf_calfunc(cal, i) > p->xend; --i);
		p->xend = i;
		p->xendcal = 0;
	}

	return 1;
}

void pf_plotdata(struct pf_data* d, struct pf_plot_params* p, struct pf_cal_fit* cal) {
	int i;
	double cmin, cmax;
	double* od;
	double chn[NCHAN];
	int chmax, chmin;
	double xmin, xmax;

	char* fn;
	char* dev;

	char* xlab;

	double rot;
	
	int norm;
	int rescale;
	int marked;
	int annot;

	fn = p->fn;
	dev = p->dev;

	rot = p->rot;

	norm = p->norm;
	rescale = p->rescale;
	marked = p->marked;
	annot = p->annot;

	if (!finddomain(d, p, cal)) {
		return;
	}

	chmin = (int) floor(p->xstart);
	chmax = (int) floor(p->xend);

	if (chmax <= 0 || chmax >= NCHAN) {
		chmax = NCHAN - 1;
	}

	if (chmin == chmax || chmin < 0 || chmin >= NCHAN) {
		chmin = 0;
	}

	if (norm) {
		od = d->data_norm;
	}
	else {
		od = d->data_raw;
	}

	initgraph(fn, dev, rot);

	if (norm) {
		cmin = d->min_norm;
		cmax = d->max_norm;
	}
	else {
		cmin = d->min_raw;
		cmax = d->max_raw;
	}

	if (rescale) {
		cmin = gsl_stats_min(&od[chmin], 1, chmax - chmin + 1);
		cmax = gsl_stats_max(&od[chmin], 1, chmax - chmin + 1);
	}


	if (p->cal && cal->valid) {
		for (i = 0; i < NCHAN; ++i) {
			chn[i] = pf_calfunc(cal, i);
		}

		xlab = cal->unit;
		xmin = pf_calfunc(cal, chmin);
		xmax = pf_calfunc(cal, chmax);
	}
	else {
		for (i = 0; i < NCHAN; ++i) {
			chn[i] = i;
		}

		xlab = "Channel";
		xmin = chmin;
		xmax = chmax;
	}

	plenv(xmin, xmax, cmin, 1.1*cmax, 0, 0);

	if (norm) {
		pllab(xlab, "Normalized Counts", "Normalized Data");
	}
	else {
		pllab(xlab, "Raw Counts", "Raw Data");
	}

	plline(NCHAN, chn, od);

	if (marked) {
		markgraph(d, od, p, cal);
	}

	if (annot) {
		annotgraph(d, od, p, cal);
	}

	plend();
}

void pf_plotpeak(struct pf_data* d, struct pf_plot_params* p, struct pf_cal_fit* cal) {
	int i, pw, nfp;
	double cmin, cmax;
	double* od;
	double chn[NCHAN], fitx[NCHAN*CHANPTS], fity[NCHAN*CHANPTS];
	int chmax, chmin;
	char gtit[MAXTITLE];
	
	char* fn;
	char* dev;
	
	double rot;
	
	int peak;
	
	int rescale;
	int marked;
	int annot;

	fn = p->fn;
	dev = p->dev;

	rot = p->rot;

	peak = p->peak;

	rescale = p->rescale;
	marked = p->marked;
	annot = p->annot;

        if (peak >= d->npeaks) {
		printf("valid peak numbers are: 0-%d\n", d->npeaks - 1);
		return;
	}
	if (peak < 0) {
        	peak = 0;
	}

	pw = d->peaks[peak].rbin - d->peaks[peak].lbin + 1;

	if (!finddomain(d, p, cal)) {
		return;
	}

	chmin = (int) floor(p->xstart);
	chmax = (int) floor(p->xend);

	if (chmin == chmax) {
		chmin = d->peaks[peak].lbin - pw;
		chmax = d->peaks[peak].rbin + pw;
	}

	if (chmax <= 0 || chmax >= NCHAN) {
		chmax = NCHAN - 1;
	}

	if (chmin < 0 || chmin >= NCHAN) {
		chmin = 0;
	}

	od = d->data_raw;

	initgraph(fn, dev, rot);

	cmin = d->min_raw;
	cmax = d->max_raw;

	if (rescale) {
		cmin = gsl_stats_min(&od[chmin], 1, chmax - chmin + 1);
		cmax = gsl_stats_max(&od[chmin], 1, chmax - chmin + 1);
	}

	plenv(chmin, chmax, cmin, 1.1*cmax, 0, 0);

	snprintf(gtit, MAXTITLE, "Peak %d", peak);

	if (p->cal && cal->valid) {
		for (i = 0; i < NCHAN; ++i) {
			chn[i] = pf_calfunc(cal, i);
		}

		pllab(cal->unit, "Raw Counts", gtit);
	}
	else {
		for (i = 0; i < NCHAN; ++i) {
			chn[i] = i;
		}

		pllab("Channel", "Raw Counts", gtit);
	}

	plline(NCHAN, chn, od);

	if (marked) {
		markgraph(d, od, p, cal);

		plcol0(14);

		for (i = 0; i < NCHAN*CHANPTS; ++i) {
			double x;

			x = ((double) i)/ ((double) CHANPTS);
			if (x >= d->peaks[peak].lbound && x <= d->peaks[peak].rbound) {
				double vertx[4];
				double verty[4];

				if (p->cal && cal->valid) {
					vertx[0] = vertx[1] = pf_calfunc(cal, x);
					vertx[2] = vertx[3] = pf_calfunc(cal, x + 1.0/(CHANPTS));
				}
				else {
					vertx[0] = vertx[1] = x;
					vertx[2] = vertx[3] = x + 1.0/(CHANPTS);
				}

				verty[0] = pf_bgfunc(d, peak, x);
				verty[1] = pf_fitfunc(d, peak, x);

				verty[2] = pf_fitfunc(d, peak, x + 1.0/(CHANPTS));
				verty[3] =  pf_bgfunc(d, peak, x + 1.0/(CHANPTS));

				plfill(4, vertx, verty);
			}
		}
	}

	plcol0(9);

	nfp = 0;
	for (i = 0; i < NCHAN*CHANPTS; ++i) {
		double x;

		x = ((double) i)/ ((double) CHANPTS);

		if (x >= d->peaks[peak].lbound && x <= d->peaks[peak].rbound) {
			if (p->cal && cal->valid) {
				fitx[nfp] = pf_calfunc(cal, x);
			}
			else {
				fitx[nfp] = x;
			}

			fity[nfp] = pf_fitfunc(d, peak, x);
			++nfp;
		}
	}

	plline(nfp, fitx, fity);

	plcol0(3);

	nfp = 0;
	for (i = 0; i < NCHAN*CHANPTS; ++i) {
		double x;

		x = ((double) i)/ ((double) CHANPTS);
		if (x >= d->peaks[peak].bglbound && x <= d->peaks[peak].bgrbound) {
			if (p->cal && cal->valid) {
				fitx[nfp] = pf_calfunc(cal, x);
			}
			else {
				fitx[nfp] = x;
			}

			fity[nfp] = pf_bgfunc(d, peak, x);
			++nfp;
		}
	}

	plline(nfp, fitx, fity);

	if (annot) {
		annotgraph(d, od, p, cal);
	}

	plend();
}

