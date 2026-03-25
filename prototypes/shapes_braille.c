/* shapes_braille.c — draw oval, star, trapezium and triangle using braille.
 * All shapes are drawn in dot space (2x cols, 4x rows) and packed into
 * braille codepoints for high-resolution terminal output.
 * Demonstrates canvas_line() and canvas_ellipse() primitives.
 *
 * See: ../docs/renderer-decision.md — braille dot encoding and virtual canvas
 * See: ../src/canvas.h              — canvas API
 */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DCOLS 400
#define DROWS 200

/* dot canvas: 2 dots wide x 4 dots tall per terminal cell */
static unsigned char dots[DROWS][DCOLS];

static const int dot_bit[4][2] = {
    {0, 3}, {1, 4}, {2, 5}, {6, 7}
};

static void dot_set(int dr, int dc) {
    if (dr < 0 || dr >= DROWS || dc < 0 || dc >= DCOLS) return;
    dots[dr][dc] = 1;
}

static void print_dots(int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
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

/* draw ellipse outline in dot space */
static void ellipse(float cx, float cy, float rx, float ry) {
    int steps = (int)(2 * M_PI * fmaxf(rx, ry) * 2);
    for (int i = 0; i < steps; i++) {
        float a = 2 * M_PI * i / steps;
        dot_set((int)(cy + sinf(a) * ry), (int)(cx + cosf(a) * rx));
    }
}

/* draw line in dot space */
static void line(float x0, float y0, float x1, float y1) {
    float dx = x1 - x0, dy = y1 - y0;
    int steps = (int)(sqrtf(dx*dx + dy*dy) * 2);
    if (steps == 0) return;
    for (int i = 0; i <= steps; i++) {
        float t = (float)i / steps;
        dot_set((int)(y0 + dy*t), (int)(x0 + dx*t));
    }
}

int main(void) {
    struct winsize w = {24, 80, 0, 0};
    int fd = open("/dev/tty", O_RDONLY);
    if (fd >= 0) { ioctl(fd, TIOCGWINSZ, &w); close(fd); }
    int cols = (w.ws_col > 0 ? w.ws_col : 80);
    int rows = (w.ws_row > 0 ? w.ws_row - 1 : 23);

    int dw = cols * 2, dh = rows * 4;
    int hw = dw / 2, hh = dh / 2;

    memset(dots, 0, sizeof(dots));

    /* quadrant offsets */
    float ox[4] = { hw*0.5f, hw*1.5f, hw*0.5f, hw*1.5f };
    float oy[4] = { hh*0.5f, hh*0.5f, hh*1.5f, hh*1.5f };
    float qrx = hw * 0.4f;
    float qry = hh * 0.4f;

    /* Q1: oval (wide ellipse, rx >> ry) */
    ellipse(ox[0], oy[0], qrx, qry * 0.5f);

    /* Q2: 5-point star using inner (r2) and outer (r1) radius */
    {
        float cx = ox[1], cy = oy[1];
        float r1 = qrx * 0.9f, r2 = qrx * 0.4f;
        int n = 5;
        for (int i = 0; i < n; i++) {
            float a0 = 2*M_PI*i/n - M_PI/2;
            float a1 = 2*M_PI*i/n + M_PI/n - M_PI/2;
            float a2 = 2*M_PI*(i+1)/n - M_PI/2;
            float px = cx + cosf(a0)*r1, py = cy + sinf(a0)*r1;
            float mx = cx + cosf(a1)*r2, my = cy + sinf(a1)*r2;
            float nx = cx + cosf(a2)*r1, ny = cy + sinf(a2)*r1;
            line(px, py, mx, my);
            line(mx, my, nx, ny);
        }
    }

    /* Q3: trapezium — wider base (bw) than top (tw) */
    {
        float cx = ox[2], cy = oy[2];
        float tw = qrx * 0.6f, bw = qrx * 0.9f, h = qry * 0.7f;
        line(cx-bw, cy+h, cx+bw, cy+h); /* bottom */
        line(cx-tw, cy-h, cx+tw, cy-h); /* top */
        line(cx-bw, cy+h, cx-tw, cy-h); /* left */
        line(cx+bw, cy+h, cx+tw, cy-h); /* right */
    }

    /* Q4: equilateral-ish triangle with apex at top */
    {
        float cx = ox[3], cy = oy[3];
        float r = qrx * 0.9f;
        float ax = cx,          ay = cy - r;
        float bx = cx - r*0.9f, by = cy + r*0.6f;
        float ccx = cx + r*0.9f, ccy = cy + r*0.6f;
        line(ax, ay, bx, by);
        line(bx, by, ccx, ccy);
        line(ccx, ccy, ax, ay);
    }

    print_dots(rows, cols);
    return 0;
}
