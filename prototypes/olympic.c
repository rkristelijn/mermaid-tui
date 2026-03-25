/* olympic.c — render the 5 Olympic rings using the braille canvas.
 * Top row: 3 rings. Bottom row: 2 rings offset by half a gap.
 * Rings overlap slightly (gap < 2*r) to match the real logo. */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../src/canvas.h"

int main(void) {
    struct winsize w = {24, 80, 0, 0};
    int fd = open("/dev/tty", O_RDONLY);
    if (fd >= 0) { ioctl(fd, TIOCGWINSZ, &w); close(fd); }
    int cols = w.ws_col > 0 ? w.ws_col : 80;
    int rows = w.ws_row > 0 ? w.ws_row - 1 : 23;

    canvas_init(cols, rows);

    float dw = canvas_dw, dh = canvas_dh;
    float r  = dh * 0.13f;          /* ring radius */
    float cx = dw * 0.5f;           /* center x */
    float cy = dh * 0.35f;          /* center y */
    float gap = r * 1.7f;           /* more overlap */
    float row2 = cy + r * 1.4f;     /* bottom row y */

    /* top row: 3 rings */
    canvas_ellipse(cx - gap,  cy, r, r);
    canvas_ellipse(cx,        cy, r, r);
    canvas_ellipse(cx + gap,  cy, r, r);

    /* bottom row: 2 rings (offset) */
    canvas_ellipse(cx - gap * 0.5f, row2, r, r);
    canvas_ellipse(cx + gap * 0.5f, row2, r, r);

    canvas_print_braille();
    return 0;
}
