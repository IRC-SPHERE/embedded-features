#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define FILENAME "combinedall.bin"

#define SCALE (256.0 / 8.0)

int main(int argc, const char *argv[])
{
  const char *filename = FILENAME;
  if (argc > 1) {
    filename = argv[1];
  }

  FILE *f = fopen(filename, "rb");
  signed char x, y, z;
  int c = 0;
  while(fscanf(f, "%c%c%c", &x, &y, &z) == 3) {
    printf("%d: %0.5f %0.5f %0.5f\n", c, x / SCALE, y / SCALE, z / SCALE);
    c++;
  }
}
