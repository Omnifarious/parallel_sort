#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

const uint32_t total_size = 1ULL << 31;

int compare_ints(void const *a, void const *b)
{
   const uint32_t ta = *((uint32_t *)a);
   const uint32_t tb = *((uint32_t *)b);
   return (ta > tb) ? 1 : ((ta == tb) ? 0 : -1);
}

uint32_t get_urandom()
{
   FILE *urandom = fopen("/dev/urandom", "rb");
   char buf[4];
   fread(buf, 1U, 4, urandom);
   fclose(urandom);
   return ((uint32_t)(buf[0]) << 24) |
      ((uint32_t)(buf[1]) << 16) |
      ((uint32_t)(buf[2]) << 8) |
      (uint32_t)(buf[3]);
}

int main()
{
   uint32_t * const huge_array = calloc(total_size, sizeof(uint32_t));
   uint32_t seed = get_urandom();
   seed = (seed * 48271) % 2147483647ULL;
   for (uint32_t i = 0; i < total_size; ++i) {
      huge_array[i] = seed;
      seed = (seed * 48271) % 2147483647ULL;
   }
   qsort(huge_array, total_size, sizeof(huge_array[0]), compare_ints);
}
