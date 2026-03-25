/* circles4.c — draw 4 circles using different ASCII char sets.
 * Each quadrant uses a different set of characters to represent the circle outline.
 * Characters are chosen based on the angle of the point (8 sectors of 45°).
 * Uses a char grid buffer to avoid ANSI cursor positioning.
 *
 * See: ../docs/renderer-decision.md — comparison of ASCII vs braille rendering
 */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define MAXCOLS 300
#define MAXROWS 100

/* canvas: one char per cell (single-byte for simplicity) */
static char canvas[MAXROWS][MAXCOLS + 1];

static void canvas_clear(int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        memset(canvas[r], ' ', cols);
        canvas[r][cols] = '\0';
    }
}

static void canvas_set(int r, int c, char ch) {
    if (r >= 0 && r < MAXROWS && c >= 0 && c < MAXCOLS)
        canvas[r][c] = ch;
}

static void canvas_print(int rows) {
    for (int r = 0; r < rows; r++)
        printf("%s\n", canvas[r]);
}

static void draw_circle(float cx, float cy, float rx, float ry,
                        const char *chars[8], float thickness) {
    int r0 = (int)(cy - ry - 2), r1 = (int)(cy + ry + 2);
    int c0 = (int)(cx - rx - 2), c1 = (int)(cx + rx + 2);
    for (int r = r0; r <= r1; r++) {
        for (int c = c0; c <= c1; c++) {
            float dx = (c - cx) / rx;
            float dy = (r - cy) / ry;
            float dist = sqrtf(dx*dx + dy*dy);
            if (fabsf(dist - 1.0f) < thickness / fminf(rx, ry * 2)) {
                /* map angle to one of 8 sectors (N/NE/E/SE/S/SW/W/NW) */
                float angle = atan2f(dy * ry, dx * rx);
                int sector = (int)((angle + M_PI) / (2 * M_PI) * 8 + 0.5f) % 8;
                canvas_set(r, c, chars[sector][0]);
            }
        }
    }
}

int main(void) {
    struct winsize w = {24, 80, 0, 0};
    int fd = open("/dev/tty", O_RDONLY);
    if (fd >= 0) { ioctl(fd, TIOCGWINSZ, &w); close(fd); }
    int cols = (w.ws_col > 0 ? w.ws_col : 80);
    int rows = (w.ws_row > 0 ? w.ws_row - 1 : 23);
    if (cols > MAXCOLS) cols = MAXCOLS;
    if (rows > MAXROWS) rows = MAXROWS;

    canvas_clear(rows, cols);

    int hw = cols / 2, hh = rows / 2;
    float rx = hw / 2.0f - 2;
    float ry = hh / 2.0f - 1;
    float t  = 0.5f;

    float cx[4] = { hw*0.5f, hw*1.5f, hw*0.5f, hw*1.5f };
    float cy[4] = { hh*0.5f, hh*0.5f, hh*1.5f, hh*1.5f };

    /* N  NE   E    SE    S    SW   W    NW  */
    const char *sets[4][8] = {        { "|", "/",  "-",  "\\", "|", "/",  "-",  "\\" },
        { "|", "/",  "=",  "\\", "|", "/",  "=",  "\\" },
        { ":", "\"", "~",  "`",  ":", "'",  "~",  "\"" },
        { "o", "o",  "o",  "o",  "o", "o",  "o",  "o"  },
    };

    for (int q = 0; q < 4; q++)
        draw_circle(cx[q], cy[q], rx, ry, sets[q], t);

    canvas_print(rows);
    return 0;
}
