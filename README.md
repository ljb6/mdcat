# `mdcat` - a minimalist Markdown renderer

`mdcat` is a fast, zero-dependency Markdown renderer for the terminal, written in C. It reads a `.md` file and pretty-prints it with ANSI colors and formatting directly to standard output.

## Features

- **Headings** — H1–H3 rendered in distinct colors (blue, cyan, yellow); H4+ fall back to reset
- **Bold headings** — every heading is rendered bold
- **Fenced code blocks** — multi-line ` ``` ` blocks are accumulated and printed with a gray background, padded to a uniform width
- **Unordered lists** — `-`, `*`, and `+` markers; nested lists (indent of 2 spaces per level) with distinct Unicode bullets (`•`, `◦`, `▪`, `▸`)
- **Paragraphs** — plain text is passed through unchanged
- **Blank-line preservation** — empty lines produce a single blank line in output

## Installation

```sh
git clone https://github.com/ljb6/mdcat
cd mdcat
make
```

This produces a single `mdcat` binary in the project directory. No external libraries or runtime dependencies are required — just a C compiler (`gcc`).

## Usage

```sh
./mdcat README.md
```

Pass any `.md` (or `.MD`) file as the sole argument. The rendered output is written to stdout, so you can pipe it freely:

```sh
./mdcat notes.md | less -R
```

### Error cases

| Situation | Behaviour |
|---|---|
| Wrong number of arguments | Prints usage to stderr and exits with failure |
| File does not end in `.md` / `.MD`  | Prints error to stderr and exits with failure |
| File cannot be opened | Prints error to stderr and exits with failure |
| A line exceeds 4096 characters | Prints error to stderr and exits with failure |
| Code block exceeds 100 lines | Prints error to stderr and exits with failure |

## Makefile targets

| Target | Description |
|---|---|
| `make` / `make all` | Compile `mdcat` with `-Wall -Wextra -g` |
| `make sanitize` | Compile `mdcat_sanitize` with AddressSanitizer enabled |
| `make clean` | Remove compiled binaries and debug symbol bundles |
| `make format` | Auto-format source with `clang-format` |

## Implementation notes

- Maximum line length: **4 096 bytes** (`MAX_LINE_LENGTH`)
- Maximum lines per fenced code block: **100** (`MAX_CODE_LINES`)
- Code blocks are padded to the width of the longest line, giving them a clean rectangular appearance
- List nesting depth is inferred from the leading-space count (2 spaces = 1 level)
- ANSI escape sequences used: bold, foreground colors (blue / cyan / yellow / white), background gray, and reset

## CI

A GitHub Actions workflow (`.github/workflows/build.yml`) runs `make` on `macos-latest` for every push and pull request.

## License

MIT — see [LICENSE](LICENSE) for details.
