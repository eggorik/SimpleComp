#include "sc_variables.h"
#include <mySimpleComputer.h>

int
sc_memoryGet (int address, int *value)
{
  if (!value)
    {
      return -1;
    }
  if (address >= SC_MEMORY_SIZE || address < 0)
    {
      SC_BIT_SET (sc_register, SC_OUT_OF_RANGE);
      return -1;
    }

  if (sc_cache_ignore)
    *value = sc_memory[address];
  else
    {
      int cache_indx, addr_num;
      sc_memoryController (address, &cache_indx, &addr_num);
      *value = cache[cache_indx].values[addr_num];
    }
  return 0;
}
