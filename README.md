# `mdcat` — minimalist Markdown renderer for the terminal

`mdcat` is a fast, zero-dependency Markdown renderer written in C. It reads a `.md` file and pretty-prints it with ANSI colors and formatting directly to standard output.

## Features

### Block elements

- **Headings** — H1–H3 rendered in distinct colors (blue, cyan, yellow); H4+ fall back to default
- **Fenced code blocks** — accumulated and printed with a gray background, padded to uniform width
- **Unordered lists** — `-`, `*`, and `+` markers; nested lists with distinct Unicode bullets (`•`, `◦`, `▪`, `▸`)
- **Paragraphs** — plain text passed through with inline formatting applied
- **Blank lines** — preserved as single blank lines in output

### Inline formatting

- **Bold** — `**text**`
- **Italic** — `*text*`
- **Bold + italic** — `***text***`
- **Strikethrough** — `~~text~~`
- **Inline code** — `` `text` ``
- **Links** — `[label](url)` rendered as label with URL alongside

## Installation

```sh
git clone https://github.com/ljb6/mdcat
cd mdcat
make
```

This produces a single `mdcat` binary. No external libraries or runtime dependencies are required — just a C compiler (`gcc`).

## Usage

```sh
./mdcat file.md
```

Pass any `.md` (or `.MD`) file as the sole argument. Output is written to stdout, so it can be piped freely:

```sh
./mdcat notes.md | less -R
```

### Error cases

| Situation | Behaviour |
|---|---|
| Wrong number of arguments | Prints usage to stderr and exits with failure |
| File does not end in `.md` / `.MD` | Prints error to stderr and exits with failure |
| File cannot be opened | Prints error to stderr and exits with failure |
| A line exceeds 4096 characters | Prints error to stderr and exits with failure |
| Code block exceeds 100 lines | Prints error to stderr and exits with failure |

## Makefile targets

| Target | Description |
|---|---|
| `make` / `make all` | Compile `mdcat` with `-Wall -Wextra -g` |
| `make sanitize` | Compile with AddressSanitizer enabled |
| `make clean` | Remove compiled binaries and debug symbol bundles |
| `make format` | Auto-format source with `clang-format` |

## Implementation notes

- Maximum line length: **4 096 bytes** (`MAX_LINE_LENGTH`)
- Maximum lines per fenced code block: **100** (`MAX_CODE_LINES`)
- Inline formatting is handled by a single-pass `render_inline` function that walks the text char by char and detects markers in priority order
- Unclosed inline markers are printed as literal text
- Code blocks are padded to the width of the longest line for a clean rectangular appearance
- List nesting depth is inferred from leading-space count (2 spaces = 1 level)

## CI

A GitHub Actions workflow (`.github/workflows/build.yml`) runs `make` on `macos-latest` for every push and pull request.

## License

MIT — see [LICENSE](LICENSE) for details.
