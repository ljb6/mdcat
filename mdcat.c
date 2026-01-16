#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 4096

void process_line(const char *line)
{
}

int process_fptr(FILE *fptr)
{
  char line[MAX_LINE_LENGTH];

  while (fgets(line, sizeof(line), fptr))
  {
    size_t line_len = strlen(line);

    if (line_len > 0 && line[line_len - 1] != '\n' && !feof(fptr))
    {
      fprintf(stderr, "Error: file has lines too long\n");
      return EXIT_FAILURE;
    }

    process_line(line);
  }

  return EXIT_SUCCESS;
}

int process_file(const char *filename)
{
  FILE *fptr = fopen(filename, "r");

  if (fptr == NULL)
  {
    fprintf(stderr, "Error while opening file %s\n", filename);
    return EXIT_FAILURE;
  }

  if (strstr(filename, ".md") == NULL)
  {
    fprintf(stderr, "Invalid file format. Use a .md file\n");
  }

  process_fptr(fptr);

  fclose(fptr);
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