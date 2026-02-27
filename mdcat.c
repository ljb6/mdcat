#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 4096
static size_t max_code_lines = 100;

/* line styles */
#define ANSI_RESET "\033[0m"
#define ANSI_BOLD "\033[1m"
#define ANSI_ITALIC "\033[3m"
#define ANSI_STRIKETHROUGH "\033[9m"
#define ANSI_DIM "\033[2m"

/* foreground colors */
#define ANSI_BLUE "\033[34m"
#define ANSI_CYAN "\033[36m"
#define ANSI_FG_WHITE "\033[97m"
#define ANSI_YELLOW "\033[33m"

/* background colors */
#define ANSI_BG_GRAY "\033[100m"

typedef struct {
  int is_code_block;
  char (*code_accumulator)[MAX_LINE_LENGTH];
  int current_code_line;
  int max_width;
} MarkdownContext;

int
find_closing (const char *text, int start, char *marker, int marker_len)
{
  for (int i = start; i < (int)strlen (text); i++) {
    if (strncmp (&text[i], marker, marker_len) == 0) {
      return i;
    }
  }

  return -1;
}

void
render_inline (const char *text, const char *base_style)
{
  for (int i = 0; i < (int)strlen (text); i++) {
    if (text[i] == '`') {
      int j = find_closing (text, i + 1, "`", 1);
      if (j != -1 && j > i + 1) {
        printf ("%s%s", ANSI_BG_GRAY, ANSI_FG_WHITE);
        printf ("%.*s", j - (i + 1), &text[i + 1]);
        printf ("%s%s", ANSI_RESET, base_style);
        i = j;
      } else {
        putchar ('`');
      }
    } else if (text[i] == '*' && text[i + 1] == '*' && text[i + 2] == '*') {
      int j = find_closing (text, i + 3, "***", 3);
      if (j != -1) {
        printf ("%s%s", ANSI_BOLD, ANSI_ITALIC);
        printf ("%.*s", j - (i + 3), &text[i + 3]);
        printf ("%s%s", ANSI_RESET, base_style);
        i = j + 2;
      } else {
        putchar (text[i]);
      }
    } else if (text[i] == '*' && text[i + 1] == '*') {
      int j = find_closing (text, i + 2, "**", 2);
      if (j != -1) {
        printf ("%s", ANSI_BOLD);
        printf ("%.*s", j - (i + 2), &text[i + 2]);
        printf ("%s%s", ANSI_RESET, base_style);
        i = j + 1;
      } else {
        putchar (text[i]);
      }
    } else if (text[i] == '*') {
      int j = find_closing (text, i + 1, "*", 1);
      if (j != -1) {
        printf ("%s", ANSI_ITALIC);
        printf ("%.*s", j - (i + 1), &text[i + 1]);
        printf ("%s%s", ANSI_RESET, base_style);
        i = j;
      } else {
        putchar (text[i]);
      }
    } else if (text[i] == '~' && text[i + 1] == '~') {
      int j = find_closing (text, i + 2, "~~", 2);
      if (j != -1) {
        printf ("%s", ANSI_STRIKETHROUGH);
        printf ("%.*s", j - (i + 2), &text[i + 2]);
        printf ("%s%s", ANSI_RESET, base_style);
        i = j + 1;
      } else {
        putchar (text[i]);
      }
    } else if (text[i] == '[') {
      int label_end = find_closing (text, i + 1, "]", 1);
      if (label_end != -1 && text[label_end + 1] == '(') {
        int url_end = find_closing (text, label_end + 2, ")", 1);
        if (url_end != -1) {
          printf ("%s%s%.*s%s%s", ANSI_CYAN, ANSI_BOLD, label_end - (i + 1),
                  &text[i + 1], ANSI_RESET, base_style);
          printf (" (%s%.*s%s%s)", ANSI_DIM, url_end - (label_end + 2),
                  &text[label_end + 2], ANSI_RESET, base_style);
          i = url_end;
        } else {
          putchar (text[i]);
        }
      } else {
        putchar (text[i]);
      }
    } else {
      putchar (text[i]);
    }
  }
}

void
render_header (char *content, int level)
{
  const char *color;

  switch (level) {
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

  char restore[64];
  snprintf (restore, sizeof (restore), "%s%s", color, ANSI_BOLD);
  printf ("%s%s", color, ANSI_BOLD);
  render_inline (content, restore);
  printf ("%s", ANSI_RESET);
}

void
render_list (char *content, int level)
{
  const char *bullet;
  int indent_spaces = level * 2;

  switch (level) {
  case 0:
    bullet = "•";
    break;
  case 1:
    bullet = "◦";
    break;
  case 2:
    bullet = "▪";
    break;
  default:
    bullet = "▸";
    break;
  }

  printf ("%*s%s ", indent_spaces, "", bullet);
  render_inline (content + 2, "");
}

void
render_accumulated_code (MarkdownContext *ctx)
{
  for (int i = 0; i < ctx->current_code_line; i++) {
    int current_len = strlen (ctx->code_accumulator[i]);
    int padding_needed = ctx->max_width - current_len;

    printf ("%s%s ", ANSI_BG_GRAY, ANSI_FG_WHITE);

    printf ("%s", ctx->code_accumulator[i]);

    for (int j = 0; j < padding_needed; j++) {
      putchar (' ');
    }

    printf (" %s\n", ANSI_RESET);
  }

  ctx->current_code_line = 0;
  ctx->max_width = 0;
}

void
process_line (char *line, MarkdownContext *ctx)
{
  if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') {
    printf ("\n");
    return;
  }

  if (strncmp (line, "```", 3) == 0) {
    if (ctx->is_code_block) {
      render_accumulated_code (ctx);
    }
    ctx->is_code_block = !ctx->is_code_block;
    return;
  }

  if (ctx->is_code_block) {
    /* removes null terminator */
    line[strcspn (line, "\n")] = 0;
    line[strcspn (line, "\r")] = 0;

    if ((size_t)ctx->current_code_line < max_code_lines) {
      strcpy (ctx->code_accumulator[ctx->current_code_line], line);

      int len = strlen (line);
      if (len > ctx->max_width)
        ctx->max_width = len;

      ctx->current_code_line++;
    } else {
      fprintf (stderr, "Error: code block exceeds maximum limit of %zu lines\n",
               max_code_lines);
      exit (EXIT_FAILURE);
    }

    return;
  }

  if (line[0] == '#') {
    int level = 0;
    while (line[level] == '#')
      level++;

    if (line[level] == ' ') {
      char *content = line + level + 1;
      render_header (content, level);
      return;
    } else {
      render_inline (line, "");
      return;
    }
  } else if (strncmp (line, "---", 3) == 0 || strncmp (line, "***", 3) == 0
             || strncmp (line, "___", 3) == 0) {
    for (int i = 0; i < 72; i++)
      fputs ("─", stdout);
    puts ("");
  } else {
    int indent = strspn (line, " ");
    char *content = line + indent;

    if ((content[0] == '-' || content[0] == '*' || content[0] == '+')
        && content[1] == ' ') {
      int level = indent / 2;
      render_list (content, level);
      return;
    } else {
      render_inline (line, "");
      return;
    }
  }
}

void
process_fptr (FILE *fptr)
{
  char line[MAX_LINE_LENGTH];
  MarkdownContext ctx = { 0 };
  ctx.code_accumulator
      = malloc (max_code_lines * sizeof (char[MAX_LINE_LENGTH]));
  if (ctx.code_accumulator == NULL) {
    fprintf (stderr, "error: failed to allocate memory for code accumulator\n");
    exit (EXIT_FAILURE);
  }

  while (fgets (line, sizeof (line), fptr)) {
    size_t line_len = strlen (line);

    if (line_len > 0 && line[line_len - 1] != '\n' && !feof (fptr)) {
      fprintf (stderr, "Error: file has lines too long\n");
      exit (EXIT_FAILURE);
    }

    process_line (line, &ctx);
  }
}

void
process_file (const char *filename)
{
  int len = strlen (filename);

  if (len < 3) {
    fprintf (stderr, "Filename too short. Use a .md file\n");
    exit (EXIT_FAILURE);
  }

  /* handle paths with query-like suffixes */
  const char *md_lower = strstr (filename, ".md");
  const char *md_upper = strstr (filename, ".MD");

  if (md_lower == NULL && md_upper == NULL) {
    fprintf (stderr, "Invalid file format. Use a .md file\n");
    exit (EXIT_FAILURE);
  }

  FILE *fptr = fopen (filename, "r");

  if (fptr == NULL) {
    fprintf (stderr, "Error while opening file %s\n", filename);
    exit (EXIT_FAILURE);
  }

  process_fptr (fptr);

  if (fclose (fptr) == EOF) {
    fprintf (stderr, "warning: failed to close file %s\n", filename);
  }
}

int
main (int argc, char *argv[])
{
  if (argc < 2) {
    fprintf (stderr, "Usage: %s <filename>\n", argv[0]);
    exit (EXIT_FAILURE);
  }

  int opt;
  while ((opt = getopt (argc, argv, "m:")) != -1) {
    switch (opt) {
    case 'm': {
      unsigned long value = strtoul (optarg, NULL, 10);
      if (value == 0) {
        fprintf (stderr, "error: max code lines must be greater than 0\n");
        exit (EXIT_FAILURE);
      }
      max_code_lines = value;
      break;
    }
    }
  }

  process_file (argv[optind]);

  return EXIT_SUCCESS;
}
