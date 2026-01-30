#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 4096
#define MAX_CODE_LINES 100

/* line styles */
#define ANSI_RESET "\033[0m"
#define ANSI_BOLD "\033[1m"

/* foreground colors */
#define ANSI_BLUE "\033[34m"
#define ANSI_CYAN "\033[36m"
#define ANSI_FG_WHITE "\033[97m"
#define ANSI_YELLOW "\033[33m"

/* background colors */
#define ANSI_BG_GRAY "\033[100m"

typedef enum
{
  BLOCK_HEADING,
  BLOCK_PARAGRAPH,
  BLOCK_CODE,
  BLOCK_BLANK,
} BlockType;

typedef struct
{
  BlockType type;
  int level;
  char *content;
} LineBlock;

typedef struct
{
  int is_code_block;
  char code_accumulator[MAX_CODE_LINES][MAX_LINE_LENGTH];
  int current_code_line;
  int max_width;
  int is_quote_block;
  int identation_level;
} MarkdownContext;

void render_header(const LineBlock *line_block)
{
  const char *color;

  switch (line_block->level)
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

  const char *fline = line_block->content;
  printf("%s%s%s%s", color, ANSI_BOLD, fline, ANSI_RESET);
}

void render_block(const LineBlock *line_block, const MarkdownContext *ctx)
{
  switch (line_block->type)
  {
  case BLOCK_HEADING:
  {
    render_header(line_block);
    break;
  }
  case BLOCK_CODE:
  {
    printf("%s%s%s%s", ANSI_BG_GRAY, ANSI_FG_WHITE, line_block->content, ANSI_RESET);

    break;
  }
  case BLOCK_PARAGRAPH:
  {
    printf("%s", line_block->content);
    break;
  }
  case BLOCK_BLANK:
  {
    printf("\n");
    break;
  }
  }
}

void render_accumulated_code(MarkdownContext *ctx)
{
  for (int i = 0; i < ctx->current_code_line; i++)
  {
    int current_len = strlen(ctx->code_accumulator[i]);
    int padding_needed = ctx->max_width - current_len;

    printf("%s%s ", ANSI_BG_GRAY, ANSI_FG_WHITE);

    printf("%s", ctx->code_accumulator[i]);

    for (int j = 0; j < padding_needed; j++)
    {
      putchar(' ');
    }

    printf(" %s\n", ANSI_RESET);
  }

  ctx->current_code_line = 0;
  ctx->max_width = 0;
}

void process_line(char *line, MarkdownContext *ctx)
{
  LineBlock line_block = {0};
  line_block.content = line;

  if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0')
  {
    line_block.type = BLOCK_BLANK;
    render_block(&line_block, ctx);
    return;
  }

  if (strncmp(line, "```", 3) == 0)
  {
    if (ctx->is_code_block)
    {
      render_accumulated_code(ctx);
    }
    ctx->is_code_block = !ctx->is_code_block;
    return;
  }

  if (ctx->is_code_block)
  {
    /* removes null terminator */
    line[strcspn(line, "\n")] = 0;
    line[strcspn(line, "\r")] = 0;

    if (ctx->current_code_line < MAX_CODE_LINES)
    {
      strcpy(ctx->code_accumulator[ctx->current_code_line], line);

      int len = strlen(line);
      if (len > ctx->max_width)
        ctx->max_width = len;

      ctx->current_code_line++;
    }
    return;
  }

  if (line[0] == '#')
  {
    int level = 0;
    while (line[level] == '#')
      level++;

    if (line[level] == ' ')
    {
      line_block.level = level;
      line_block.type = BLOCK_HEADING;
      line_block.content = line + level + 1;
    }
    else
    {
      line_block.type = BLOCK_PARAGRAPH;
      line_block.content = line;
    }
  }
  else
  {
    line_block.type = BLOCK_PARAGRAPH;
    line_block.content = line;
  }

  render_block(&line_block, ctx);
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
  int len = strlen(filename);
  
  if (len < 3 || strcmp(filename + len - 3, ".md") != 0)
  {
    fprintf(stderr, "Invalid file format. Use a .md file\n");
    return EXIT_FAILURE;
  }

  FILE *fptr = fopen(filename, "r");

  if (fptr == NULL)
  {
    fprintf(stderr, "Error while opening file %s\n", filename);
    return EXIT_FAILURE;
  }

  process_fptr(fptr);

  if (fclose(fptr) == EOF)
  {
    fprintf(stderr, "warning: failed to close file %s\n", filename);
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
