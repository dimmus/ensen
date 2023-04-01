#include "mem/ensen_mem_guarded.h"

#include "ensen_conf.h"

int
config_parameters_set(Signal_Parameters * param, dictionary *ini)
{
# ifdef DEBUG_CONF
  config_dump(ini, stderr);
# endif

  /* Peaks setup */
  (*param).n_points             = config_getint(ini, "points:number", -1);
  (*param).n_peaks              = config_getint(ini, "peaks:number", -1);

  (*param).peak = MEM_malloc_arrayN(4, sizeof(Peak), "config_parameters_set: peak[4]");
  (*param).peak->timeshift      = config_getint(ini, "peaks:timeshift", -1);

  (*param).peak[0].amplitude    = config_getdouble(ini, "peaks:[0].amplitude", -1.0);
  (*param).peak[0].position     = config_getdouble(ini, "peaks:[0].position", -1.0);
  (*param).peak[0].width        = config_getdouble(ini, "peaks:[0].width", -1.0);

  (*param).peak[1].amplitude    = config_getdouble(ini, "peaks:[1].amplitude", -1.0);
  (*param).peak[1].position     = config_getdouble(ini, "peaks:[1].position", -1.0);
  (*param).peak[1].width        = config_getdouble(ini, "peaks:[1].width", -1.0);

  (*param).peak[2].amplitude    = config_getdouble(ini, "peaks:[2].amplitude", -1.0);
  (*param).peak[2].position     = config_getdouble(ini, "peaks:[2].position", -1.0);
  (*param).peak[2].width        = config_getdouble(ini, "peaks:[2].width", -1.0);

  (*param).peak[3].amplitude    = config_getdouble(ini, "peaks:[3].amplitude", -1.0);
  (*param).peak[3].position     = config_getdouble(ini, "peaks:[3].position", -1.0);
  (*param).peak[3].width        = config_getdouble(ini, "peaks:[3].width", -1.0);

  /* Noise setup */
  (*param).noise.amplitude      = config_getdouble(ini, "noise:amplitude", -1.0);
  (*param).noise.color          = config_getint(ini, "noise:color", -1);

  /* Smooth setup */
  (*param).smooth.width         = config_getint(ini, "smooth:width", -1);
  (*param).smooth.level         = config_getint(ini, "smooth:level", -1);

  /* Generation setup */
  (*param).generation_max       = config_getint(ini, "generation:number", -1.0);
  (*param).generation_frequency = config_getint(ini, "generation:frequency", -1.0); // Hz

  /* Temperature setup */
  (*param).temp.apply           = config_getint(ini, "temperature:apply", -1.0);
  (*param).temp.tick            = config_getint(ini, "temperature:tick", -1.0);
  (*param).temp.room            = config_getdouble(ini, "temperature:room", -1.0);
  (*param).temp.max             = config_getdouble(ini, "temperature:max", -1.0);
  (*param).temp.coefficient     = config_getdouble(ini, "temperature:coefficient", -1.0);

  /* Search setup */
  (*param).search.threshold_slope     = config_getdouble(ini, "search:threshold_slope", -1.0);
  (*param).search.threshold_amp       = config_getdouble(ini, "search:threshold_amp", -1.0);
  (*param).search.peaks_real_number   = config_getint(ini, "search:peaks.num.real", -1.0);
  (*param).search.peaks_array_number  = config_getint(ini, "search:peaks.num.arr", -1.0);
  (*param).search.peak_search_number  = config_getint(ini, "search:peaks.num", -1.0);

  /* Plot setup */
  (*param).plot.x_min             = config_getdouble(ini, "plot:x.min", -1.0);
  (*param).plot.x_max             = config_getdouble(ini, "plot:x.max", -1.0);
  (*param).plot.y_min             = config_getdouble(ini, "plot:y.min", -1.0);
  (*param).plot.y_max             = config_getdouble(ini, "plot:y.max", -1.0);
  (*param).plot.show_signal       = config_getint(ini, "plot:show.signal", -1.0);
  (*param).plot.show_smooth       = config_getint(ini, "plot:show.signal.smooth", -1.0);
  (*param).plot.show_derivative   = config_getint(ini, "plot:show.signal.derivative", -1.0);
  (*param).plot.show_markers      = 0; /* deprecated */
  (*param).plot.show_temperature  = config_getint(ini, "plot:show.experiment.temperature", -1.0);
  (*param).plot.show_vs_smooth    = config_getint(ini, "plot:show.experiment.smooth", -1.0);
  (*param).plot.show_vs_noise     = config_getint(ini, "plot:show.experiment.noise", -1.0);

  return 0;
}

void
config_parameters_set_default(void)
{
    FILE *ini ;

    if ((ini=fopen("config.ini", "w"))==NULL) {
        fprintf(stderr, "config: cannot create config.ini\n");
        return ;
    }

    fprintf(ini,
    "[Points]\n"
    "number          = 15000;        // total number of points\n"
    "\n"
    "[Peaks]\n"
    "number          = 4;            // number of peaks to generate\n"
    "timeshift       = 500;          // parameter for exp_gaussian function\n"
    "[0].amplitude   = 1.0;          // amplitude of peak 1 (a.u.)\n"
    "[0].position    = 1511.0;       // position of peak 1 (nm)\n"
    "[0].width       = 1.0;          // FWHM of peak 1 (nm)\n"
    "\n"
    "[1].amplitude   = 0.8;          // amplitude of peak 2 (a.u.)\n"
    "[1].position    = 1523.0;       // position of peak 2 (nm)\n"
    "[1].width       = 1.0;          // FWHM of peak 2 (nm)\n"
    "\n"
    "[2].amplitude   = 0.6;          // amplitude of peak 3 (a.u.)\n"
    "[2].position    = 1536.0;       // position of peak 3 (nm)\n"
    "[2].width       = 1.0;          // FWHM of peak 3 (nm)\n"
    "\n"
    "[3].amplitude   = 0.4;          // amplitude of peak 4 (a.u.)\n"
    "[3].position    = 1551.0;       // position of peak 4 (nm)\n"
    "[3].width       = 1.0;          // FWHM of peak 4 (nm)\n"
    "\n"
    "[Noise]\n"
    "amplitude       = 0.15;         // noise amplitude (a.u.)\n"
    "color           = 3;            // 0-random; 1-white; 2-brown; 3-violet; 4-pink (unfinished); 5-blue (unfinished)\n"
    "\n"
    "[Smooth]\n"
    "width           = 100;          // smooth width (points)\n"
    "level           = 3;            // number of smooth operations to apply\n"
    "\n"
    "[Search]\n"
    "peaks.num       = 1;            // peak of interest (sensor) number to show on graph\n"
    "threshold_slope = 0.000001;     // first derivative slope threshold\n"
    "threshold_amp   = 0.35;         // signal's peak amplitude threshold\n"
    "peaks.num.real  = 4;            // number of perak to search for\n"
    "peaks.num.arr   = 10;           // maximal array size for peak searching\n"
    "\n"
    "[Generation]\n"
    "number          = 100;          // number of signal generations (mesurements in experiment)\n"
    "frequency       = 5;            // frequency of mesurements\n"
    "\n"
    "[Temperature]\n"
    "apply           = 1;            // calculate temperature drift\n"
    "tick            = 20;           // number of mesurement to change temperature\n"
    "room            = 20;           // room temperature\n"
    "max             = 1350;         // maxumum temperature in experiment\n"
    "coefficient     = 0.02;         // coefficient [nm/C]\n"
    "\n"
    "[Plot]\n"
    "x.min           = 1500;         // x window limit (left)\n"
    "x.max           = 1600;         // x window limit (right)\n"
    "y.min           = 0.0;          // y window limit (bottom)\n"
    "y.max           = 1.4;          // y window limit (top)\n"
    "show.signal     = 1;             // show plot of real generated signal\n"
    "show.signal.smooth     = 0;      // show plot of smoothed generated signal\n"
    "show.signal.derivative = 0;      // show plot of fisrt derivative of generated signal\n"
    "show.experiment.temperature = 0; // show plot of temperature change on sensors\n"
    "show.experiment.smooth      = 0; // show plot of temperature vs smooth (1 - order, 2 - width)\n"
    "show.experiment.noise       = 1; // show plot of temperature vs noise (SNR)\n"
    "\n"
    );
    fclose(ini);
}
