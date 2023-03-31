#include "ensen_show.h"

void
show_statistics(Signal_Parameters conf, Signal_Statistics stat, Points temp_gen, Points temp_sens_1, Points temp_sens_2, Points temp_sens_3, Points temp_sens_4)
{
  index_t i = 0;
  printf("\n");
  printf(BLUE("STATISTUS:")" Generation time:\t%f sec\n", stat.generation_time); // the last generation time
  printf(BLUE("STATISTUS:")" Max search freq:\t%f kHz\n", (1/stat.peak_search_time)/1000); // the last generation time
  printf(BLUE("STATISTUS:")" Number of drops:\t%d\n", stat.n_drops);
  if (conf.noise.amplitude > 0)
  {
    if (conf.noise.color == 1) printf(BLUE("STATISTUS:")" Noise color    :\t1 - white\n");
    if (conf.noise.color == 2) printf(BLUE("STATISTUS:")" Noise color    :\t2 - brown\n");
    if (conf.noise.color == 3) printf(BLUE("STATISTUS:")" Noise color    :\t3 - violet\n");
    if (conf.noise.color == 4) printf(BLUE("STATISTUS:")" Noise color    :\t4 - pink\n");
    if (conf.noise.color == 5) printf(BLUE("STATISTUS:")" Noise color    :\t5 - blue\n");
  }

  if(conf.temp.apply)
    printf(BLUE("STATISTUS:")" Rise of T[step]:\t%f C\n", stat.delta_temp);
  else
    printf(BLUE("STATISTUS:")" Rise of T[step]:\t- (no temperature rise)\n");
  printf(BLUE("STATISTUS:")" Sensor number #:\t%d\n", conf.search.peak_search_number);

  switch (conf.search.peak_search_number)
  {
    case 1:
      printf(BLUE("STATISTUS:")" End temperature:\t%f (generator: %f)\n", temp_sens_1.y[conf.generation_max], temp_gen.y[conf.generation_max]);
      break;
    case 2:
      printf(BLUE("STATISTUS:")" End temperature:\t%f (generator: %f)\n", temp_sens_2.y[conf.generation_max], temp_gen.y[conf.generation_max]);
      break;
    case 3:
      printf(BLUE("STATISTUS:")" End temperature:\t%f (generator: %f)\n", temp_sens_3.y[conf.generation_max], temp_gen.y[conf.generation_max]);
      break;
    case 4:
      printf(BLUE("STATISTUS:")" End temperature:\t%f (generator: %f)\n", temp_sens_4.y[conf.generation_max], temp_gen.y[conf.generation_max]);
      break;
  }

  data_t diff[conf.generation_max];
  switch (conf.search.peak_search_number)
  {
    case 1:
      for (i = 0; i <= conf.generation_max; i++) diff[i] = temp_sens_1.y[i] - temp_gen.y[i];
      break;
    case 2:
      for (i = 0; i <= conf.generation_max; i++) diff[i] = temp_sens_2.y[i] - temp_gen.y[i];
      break;
    case 3:
      for (i = 0; i <= conf.generation_max; i++) diff[i] = temp_sens_3.y[i] - temp_gen.y[i];
      break;
    case 4:
      for (i = 0; i <= conf.generation_max; i++) diff[i] = temp_sens_4.y[i] - temp_gen.y[i];
      break;
  }
  printf(BLUE("STATISTUS:")" Max temp deviation:\t[%f, %f]\n", max(diff, conf.generation_max + 1), min(diff, conf.generation_max + 1));
  printf(BLUE("STATISTUS:")" Min temp deviation:\t%f\n", min_abs(diff, conf.generation_max + 1));
}

void
show_generator_info(const Signal_Parameters conf, const Signal_Statistics stat, const index_t n_step)
{
  index_t i = 0;

  printf("\n" GREEN("GENERATOR:") " Temperature changed! New positions [ ");
  for (i = 0; i < conf.n_peaks; i++)
  {
    printf(CYAN("%f"), conf.peak[i].position);
    if (i != conf.n_peaks - 1) printf(", ");
  }
  printf(" ]");

  printf("\n" GREEN("GENERATOR:") " Temperature changed! The new value [ ");
  for (i = 0; i < conf.n_peaks; i++)
  {
    printf(CYAN("%f"), conf.temp.room + stat.delta_temp * n_step);
    if (i != conf.n_peaks - 1) printf(", ");
  }

  printf(" ]\n");
}

void
show_psearch_info(const Signal_Parameters conf, const index_t i_gen, const index_t i_sens, const index_t n_peaks,
                  const Points temp_sens_1, const Points temp_sens_2, const Points temp_sens_3, const Points temp_sens_4)
{
  data_t value = 0.f;

  if (i_sens == 0) printf("[ ");

  /* Show temperature */
  switch (i_sens)
  {
    case 0:
      value = temp_sens_1.y[i_gen];
      break;
    case 1:
      value = temp_sens_2.y[i_gen];
      break;
    case 2:
      value = temp_sens_3.y[i_gen];
      break;
    case 3:
      value = temp_sens_4.y[i_gen];
      break;
    default:
      break;
  }

  // format output data string
  printf((i_sens != conf.search.peaks_real_number - 1) ? "%f, " : "%f", value);

  // show the note about found and real number of peaks
  if (i_sens == conf.search.peaks_real_number - 1)
  {
    if (n_peaks == conf.search.peaks_real_number)
    {
      printf(" ] -> "GREEN("[%d of %d]\n"), conf.search.peaks_real_number, n_peaks);
    }
    else if (n_peaks < conf.search.peaks_real_number)
    {
      printf(" ] -> "YELLOW("[%d of %d]\n"), conf.search.peaks_real_number, n_peaks);
    }
    else
    {
      printf(" ] -> "RED("[%d of %d]\n"), conf.search.peaks_real_number, n_peaks);
    }
  }
}
