#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int process_file(const char *filename)
{
  FILE *fptr;

  fptr = fopen(filename, "r");

  if (fptr == NULL)
  {
    fprintf(stderr, "Error while opening file %s\n", filename);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
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