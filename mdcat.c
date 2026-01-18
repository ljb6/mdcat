#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 4096

#define ANSI_RESET "\033[0m"
#define ANSI_BOLD "\033[1m"
#define ANSI_CYAN "\033[36m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"

typedef enum
{
  TEXT,
  BOLD_START,
  BOLD_END,
  ITALIC_START,
  ITALIC_END,
  LINE_BREAK,
} TokenType;

typedef struct
{
  int is_code_block;
  int is_quote_block;
  int identation_level;
} MarkdownContext;

void render_header(const char *line, int level)
{
  const char *color;

  switch (level)
  {
  case 1:
    color = ANSI_BLUE;
    break;
  case 2:
    color = ANSI_CYAN;
    break;
  case 3:
    color = ANSI_YELLOW;
    break;
  default:
    color = ANSI_RESET;
    break;
  }

  const char *fline = line + level + 1;
  printf("%s%s%s%s", color, ANSI_BOLD, fline, ANSI_RESET);
}

void process_line(const char *line, MarkdownContext *ctx)
{
  /* code block logic*/
  if (ctx->is_code_block)
  {
    return;
  }

  if (line[0] == '#')
  {
    int level = 0;
    while (line[level] == '#')
    {
      level++;
    }

    render_header(line, level);
  }

  return;
}

int process_fptr(FILE *fptr)
{
  char line[MAX_LINE_LENGTH];
  MarkdownContext ctx = {0};

  while (fgets(line, sizeof(line), fptr))
  {
    size_t line_len = strlen(line);

    if (line_len > 0 && line[line_len - 1] != '\n' && !feof(fptr))
    {
      fprintf(stderr, "Error: file has lines too long\n");
      return EXIT_FAILURE;
    }

    process_line(line, &ctx);
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