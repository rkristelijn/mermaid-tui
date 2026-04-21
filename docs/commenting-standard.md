# C Inline Documentation Standard

## Chosen standard: Doxygen

[Doxygen](https://www.doxygen.nl/) is the de-facto standard for C/C++ inline documentation. It:

- Parses specially formatted comments and generates HTML/PDF docs
- Is supported by most editors (VSCode, CLion, vim plugins)
- Is AI-friendly — structured tags make intent explicit
- Works with plain `/* */` comments, no extra tooling needed to read

## Comment style

Use `/** */` for Doxygen doc comments (double star), `/* */` for regular comments:

```c
/**
 * Draw a line from (x0,y0) to (x1,y1) in dot space.
 *
 * Samples at 2x the Euclidean length to ensure no gaps.
 *
 * @param x0  start x in dot space
 * @param y0  start y in dot space
 * @param x1  end x in dot space
 * @param y1  end y in dot space
 * @see canvas_dot
 */
void canvas_line(float x0, float y0, float x1, float y1);
```

## Tags used in this project

| Tag | Purpose |
|-----|---------|
| `@param name desc` | Document a function parameter |
| `@return desc` | Document return value |
| `@see symbol_or_path` | Cross-reference to related symbol or doc file |
| `@note text` | Important note or caveat |
| `@file` | File-level description (top of file) |
| `@brief` | One-line summary (optional, first line is used if omitted) |

## File header template

```c
/**
 * @file canvas.c
 * @brief Braille dot canvas — drawing primitives and terminal renderer.
 *
 * Drawing happens in dot space (canvas_dw x canvas_dh), where each terminal
 * cell maps to a 2x4 braille dot block.
 *
 * @see ../docs/renderer-decision.md
 * @see ../docs/architecture.md
 */
```

## Function header template

```c
/**
 * Initialize canvas for given terminal dimensions.
 * Must be called before any drawing function.
 *
 * @param cols  terminal width in columns
 * @param rows  terminal height in rows
 */
void canvas_init(int cols, int rows);
```

## Rules

1. Every public function in a `.h` file gets a `/** */` doc comment
2. Internal `static` functions get a `/* */` comment if non-obvious
3. Every file gets a `@file` header
4. Use `@see path` to link to related docs or source files
5. Comments explain *why*, not *what* — the code shows what

## Threshold

Maintain **≥ 20% comment ratio** (measured by `cloc`):

```bash
cloc src/ | grep -E "comment|SUM"
```

Target: `comment / code >= 0.20`

## Generating docs

```bash
sudo apt install doxygen
doxygen -g   # generate Doxyfile
doxygen      # generate docs in docs/html/
```

Or add a `make docs` target:

```makefile
docs:
	doxygen Doxyfile
```
