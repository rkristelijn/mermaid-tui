# Renderer Decision

## Conclusion: Braille as primary renderer

After prototyping four approaches (ASCII, box-drawing, dots, braille), braille is the clear winner.

## Why braille

Each terminal cell contains a 2×4 braille dot grid, giving **8× more resolution** than ASCII:

| Renderer | Resolution per cell | Curves | Diagonals | Line width 1 |
|----------|--------------------|---------|-----------|----|
| ASCII (`-/\|`) | 1×1 | blocky | stepped | no |
| Box-drawing (`─│╱╲`) | 1×1 | blocky | limited | no |
| Block (`▀▄`) | 1×2 | poor | no | no |
| Braille (`⣿`) | 2×4 | smooth | smooth | yes |

## Prototypes

See `prototypes/`:

| File | What it shows |
|------|---------------|
| `rect.c` | Full-screen rectangle, terminal size detection via `/dev/tty` |
| `circle.c` | Circle with ASCII `*` |
| `circle_braille.c` | Circle with braille — smooth, high resolution |
| `circles4.c` | 4 circles with different ASCII char sets, line thickness comparison |
| `shapes_braille.c` | Oval, star, trapezium, triangle — all braille, line width 1 |

## Terminal size detection

`ioctl(TIOCGWINSZ)` fails when stdout is a pipe (no TTY). Solution: open `/dev/tty` directly:

```c
struct winsize w = {24, 80, 0, 0};
int fd = open("/dev/tty", O_RDONLY);
if (fd >= 0) { ioctl(fd, TIOCGWINSZ, &w); close(fd); }
int cols = w.ws_col > 0 ? w.ws_col : 80;
int rows = w.ws_row > 0 ? w.ws_row - 1 : 23;
```

## Braille encoding

Each cell maps to Unicode codepoint `U+2800 + bitmask`:

```text
col 0  col 1
dot1   dot4   bit 0, 3   (row 0)
dot2   dot5   bit 1, 4   (row 1)
dot3   dot6   bit 2, 5   (row 2)
dot7   dot8   bit 6, 7   (row 3)
```

UTF-8 encode: `U+2800`–`U+28FF` → 3-byte sequence `0xE2 0xA0 0x80`–`0xE2 0xA3 0xBF`.

## Virtual canvas architecture

Drawing happens in dot space (2× cols, 4× rows), then projected to terminal cells:

```text
virtual shape (float coords)
    ↓  scale to dot space
dot canvas [rows*4][cols*2]  (1 byte per dot)
    ↓  pack 8 dots → braille codepoint
terminal output [rows][cols]  (3 bytes per cell, UTF-8)
```

ASCII fallback: same dot canvas, map to `*` if any dot set, `` otherwise.
