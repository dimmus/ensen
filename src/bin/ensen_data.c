#include "ensen_data.h"

#include "mem/ensen_mem_guarded.h"

void
data_clear(data_t * x, const index_t size)
{
  for (index_t i = 0; i < size; i++)
  {
    x[i] = 0.0;
  }
}

void
data_convert_to_lambda(data_t * lambda, const data_t lambda_begin, const data_t lambda_end, const index_t size)
{
  for (index_t i = 0; i < size; i++)
  {
    lambda[i] = lambda_begin + ( i * ((lambda_end - lambda_begin)/size));
  }
}

void
data_arrays_set(Signal_Parameters conf, Points *data, Points *data_temp,
                Points *temp_gen, Points *temp_sens_1, Points *temp_sens_2, Points *temp_sens_3, Points *temp_sens_4)
{
  (*data).x = (*data).y = NULL;
  (*data).x = MEM_malloc_arrayN(conf.n_points + 1, sizeof(data_t), "data_arrays_set: data.x");
  (*data).y = MEM_malloc_arrayN(conf.n_points + 1, sizeof(data_t), "data_arrays_set: data.y");
  data_convert_to_lambda((*data).x, conf.plot.x_min, conf.plot.x_max, conf.n_points);

  (*data_temp).x = (*data_temp).y = NULL;
  (*data_temp).x = MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: data_temp.x");
  (*data_temp).y = MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: data_temp.y");

  (*temp_gen).y = NULL;
  (*temp_gen).y = MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_gen.y");

  (*temp_sens_1).x = (*temp_sens_2).x = (*temp_sens_3).x = (*temp_sens_4).x = NULL;
  (*temp_sens_1).y = (*temp_sens_2).y = (*temp_sens_3).y = (*temp_sens_4).y = NULL;
  (*temp_sens_1).y = MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_sens_1.y");
  (*temp_sens_2).y = MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_sens_2.y");
  (*temp_sens_3).y = MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_sens_3.y");
  (*temp_sens_4).y = MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_sens_4.y");
  
  (*temp_gen).y[0] = (*temp_sens_1).y[0] = (*temp_sens_2).y[0] =(* temp_sens_3).y[0] = (*temp_sens_4).y[0] = conf.temp.room;
}

