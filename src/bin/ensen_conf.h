#ifndef ENSEN_CONF_H
#define ENSEN_CONF_H

#include "signal/ensen_signal.h"
#include "config/ensen_config.h"
#include "config/ensen_config_dictionary.h"

int config_parameters_set(Signal_Parameters * param, const dictionary *ini);
void config_parameters_set_default(void);

#endif
