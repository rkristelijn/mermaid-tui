/* canvas.c — braille dot canvas
 *
 * Drawing happens in dot space (canvas_dw x canvas_dh), where each terminal
 * cell maps to a 2x4 braille dot block. Primitives set individual dots;
 * canvas_print_braille() packs 8 dots per cell into a Unicode braille codepoint.
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "canvas.h"

/* dot grid: 1 byte per dot, indexed [row][col] in dot space */
static unsigned char dots[CANVAS_MAXROWS][CANVAS_MAXCOLS];
static int term_cols, term_rows;
int canvas_dw, canvas_dh; /* dot dimensions = term_cols*2, term_rows*4 */

/* Braille dot-to-bit mapping (Unicode standard):
 *   col 0  col 1
 *   dot1   dot4   bit 0, 3   (row 0)
 *   dot2   dot5   bit 1, 4   (row 1)
 *   dot3   dot6   bit 2, 5   (row 2)
 *   dot7   dot8   bit 6, 7   (row 3)
 */
static const int dot_bit[4][2] = {
    {0, 3}, {1, 4}, {2, 5}, {6, 7}
};

/* Initialize canvas for given terminal dimensions.
 * Must be called before any drawing. */
void canvas_init(int cols, int rows) {
    term_cols = cols;
    term_rows = rows;
    canvas_dw  = cols * 2; /* 2 dots per terminal column */
    canvas_dh  = rows * 4; /* 4 dots per terminal row */
    canvas_clear();
}

/* Clear all dots. */
void canvas_clear(void) {
    memset(dots, 0, sizeof(dots));
}

/* Set a single dot at dot-space coordinates (dr, dc).
 * Silently ignored if out of bounds. */
void canvas_dot(int dr, int dc) {
    if (dr >= 0 && dr < canvas_dh && dc >= 0 && dc < canvas_dw)
        dots[dr][dc] = 1;
}

/* Draw a line from (x0,y0) to (x1,y1) in dot space using linear interpolation.
 * Samples at 2x the Euclidean length to ensure no gaps in the line. */
void canvas_line(float x0, float y0, float x1, float y1) {
    float dx = x1 - x0, dy = y1 - y0;
    /* sample at 2x the Euclidean length to avoid gaps */
    int steps = (int)(sqrtf(dx*dx + dy*dy) * 2) + 1;
    for (int i = 0; i <= steps; i++) {
        float t = (float)i / steps;
        canvas_dot((int)(y0 + dy*t), (int)(x0 + dx*t));
    }
}

/* Draw an ellipse outline centered at (cx,cy) with radii rx, ry in dot space.
 * Samples at 2x the circumference to ensure no gaps. For a circle, rx == ry. */
void canvas_ellipse(float cx, float cy, float rx, float ry) {
    /* sample at 2x the circumference to avoid gaps */
    int steps = (int)(2 * M_PI * fmaxf(rx, ry) * 2) + 1;
    for (int i = 0; i < steps; i++) {
        float a = 2 * M_PI * i / steps;
        canvas_dot((int)(cy + sinf(a) * ry), (int)(cx + cosf(a) * rx));
    }
}

/* Draw a rectangle outline with top-left (x,y), width w, height h in dot space.
 * Draws 4 lines: top, right, bottom, left. */
void canvas_rect(float x, float y, float w, float h) {
    canvas_line(x,   y,   x+w, y);   /* top */
    canvas_line(x+w, y,   x+w, y+h); /* right */
    canvas_line(x+w, y+h, x,   y+h); /* bottom */
    canvas_line(x,   y+h, x,   y);   /* left */
}

/* Render dot canvas to stdout as Unicode braille characters (U+2800–U+28FF).
 * Each terminal cell packs 8 dots into a 3-byte UTF-8 codepoint. */
void canvas_print_braille(void) {
    for (int r = 0; r < term_rows; r++) {
        for (int c = 0; c < term_cols; c++) {
            unsigned char bits = 0;
            /* pack 2x4 dot block into braille bitmask */
            for (int dr = 0; dr < 4; dr++)
                for (int dc = 0; dc < 2; dc++)
                    if (dots[r*4+dr][c*2+dc])
                        bits |= (1 << dot_bit[dr][dc]);
            /* encode U+2800+bits as 3-byte UTF-8 */
            unsigned int cp = 0x2800 + bits;
            printf("%c%c%c", 0xE0|(cp>>12), 0x80|((cp>>6)&0x3F), 0x80|(cp&0x3F));
        }
        printf("\n");
    }
}

/* Render dot canvas as plain ASCII: '*' if any dot set, ' ' otherwise.
 * Fallback for terminals without Unicode/braille font support. */
void canvas_print_ascii(void) {
    for (int r = 0; r < term_rows; r++) {
        for (int c = 0; c < term_cols; c++) {
            int any = 0;
            for (int dr = 0; dr < 4 && !any; dr++)
                for (int dc = 0; dc < 2 && !any; dc++)
                    if (dots[r*4+dr][c*2+dc]) any = 1;
            printf("%c", any ? '*' : ' ');
        }
        printf("\n");
    }
}
