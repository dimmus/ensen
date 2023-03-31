#include <stdlib.h>

#include "ensen_utils.h"

int
clearscreen(void)
{
  return system("cls||clear"); // win and linux
#if 0
  printf("\e[1;1H\e[2J"); // warning: non ISO-complient
#endif
}