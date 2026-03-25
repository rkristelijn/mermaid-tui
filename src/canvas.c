#include <stdio.h>
#include <string.h>
#include <math.h>
#include "canvas.h"

static unsigned char dots[CANVAS_MAXROWS][CANVAS_MAXCOLS];
static int term_cols, term_rows;
int canvas_dw, canvas_dh;

static const int dot_bit[4][2] = {
    {0, 3}, {1, 4}, {2, 5}, {6, 7}
};

void canvas_init(int cols, int rows) {
    term_cols = cols;
    term_rows = rows;
    canvas_dw  = cols * 2;
    canvas_dh  = rows * 4;
    canvas_clear();
}

void canvas_clear(void) {
    memset(dots, 0, sizeof(dots));
}

void canvas_dot(int dr, int dc) {
    if (dr >= 0 && dr < canvas_dh && dc >= 0 && dc < canvas_dw)
        dots[dr][dc] = 1;
}

void canvas_line(float x0, float y0, float x1, float y1) {
    float dx = x1 - x0, dy = y1 - y0;
    int steps = (int)(sqrtf(dx*dx + dy*dy) * 2) + 1;
    for (int i = 0; i <= steps; i++) {
        float t = (float)i / steps;
        canvas_dot((int)(y0 + dy*t), (int)(x0 + dx*t));
    }
}

void canvas_ellipse(float cx, float cy, float rx, float ry) {
    int steps = (int)(2 * M_PI * fmaxf(rx, ry) * 2) + 1;
    for (int i = 0; i < steps; i++) {
        float a = 2 * M_PI * i / steps;
        canvas_dot((int)(cy + sinf(a) * ry), (int)(cx + cosf(a) * rx));
    }
}

void canvas_rect(float x, float y, float w, float h) {
    canvas_line(x,   y,   x+w, y);
    canvas_line(x+w, y,   x+w, y+h);
    canvas_line(x+w, y+h, x,   y+h);
    canvas_line(x,   y+h, x,   y);
}

void canvas_print_braille(void) {
    for (int r = 0; r < term_rows; r++) {
        for (int c = 0; c < term_cols; c++) {
            unsigned char bits = 0;
            for (int dr = 0; dr < 4; dr++)
                for (int dc = 0; dc < 2; dc++)
                    if (dots[r*4+dr][c*2+dc])
                        bits |= (1 << dot_bit[dr][dc]);
            unsigned int cp = 0x2800 + bits;
            printf("%c%c%c", 0xE0|(cp>>12), 0x80|((cp>>6)&0x3F), 0x80|(cp&0x3F));
        }
        printf("\n");
    }
}

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
