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
data_arrays_set_new(const Signal_Parameters conf, PointsArray points)
{
  points.data->x = points.data->y = NULL;
  points.data->x = MEM_malloc_arrayN(conf.n_points + 1, sizeof(data_t), "data_arrays_set: data.x");
  points.data->y = MEM_malloc_arrayN(conf.n_points + 1, sizeof(data_t), "data_arrays_set: data.y");
  data_convert_to_lambda(points.data->x, conf.plot.x_min, conf.plot.x_max, conf.n_points);

  points.data_temp->x = points.data_temp->y = NULL;
  points.data_temp->x = MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: data_temp.x");
  points.data_temp->y = MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: data_temp.y");

  points.temp_gen->y = NULL;
  points.temp_gen->y = MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_gen.y");

  points.temp_sens_1->x = points.temp_sens_2->x = points.temp_sens_3->x = points.temp_sens_4->x = NULL;
  points.temp_sens_1->y = points.temp_sens_2->y = points.temp_sens_3->y = points.temp_sens_4->y = NULL;
  points.temp_sens_1->y = MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_sens_1.y");
  points.temp_sens_2->y = MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_sens_2.y");
  points.temp_sens_3->y = MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_sens_3.y");
  points.temp_sens_4->y = MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_sens_4.y");
  
  points.temp_gen->y[0] = points.temp_sens_1->y[0] 
                        = points.temp_sens_2->y[0] 
                        = points.temp_sens_3->y[0] 
                        = points.temp_sens_4->y[0] 
                        = conf.temp.room;
}


