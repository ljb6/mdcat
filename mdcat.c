#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_file(const char *filename)
{
  printf("Filename: %s\n", filename);
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return EXIT_FAILURE;
  }

  process_file(argv[1]);

  return EXIT_SUCCESS;
}