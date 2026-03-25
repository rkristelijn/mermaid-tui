#include <stdio.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

/*
 * Braille block: each terminal cell = 2 dots wide x 4 dots tall
 * Braille unicode: U+2800 + bitmask
 * Dot layout per cell:
 *   col 0  col 1
 *   dot1   dot4   (row 0)
 *   dot2   dot5   (row 1)
 *   dot3   dot6   (row 2)
 *   dot7   dot8   (row 3)
 */
static const int dot_bit[4][2] = {
    {0, 3}, /* row 0: dot1=bit0, dot4=bit3 */
    {1, 4}, /* row 1: dot2=bit1, dot5=bit4 */
    {2, 5}, /* row 2: dot3=bit2, dot6=bit5 */
    {6, 7}, /* row 3: dot7=bit6, dot8=bit7 */
};

static void print_braille(unsigned char bits) {
    unsigned int cp = 0x2800 + bits;
    /* encode UTF-8 */
    printf("%c%c%c", 0xE0 | (cp >> 12), 0x80 | ((cp >> 6) & 0x3F), 0x80 | (cp & 0x3F));
}

int main(void) {
    struct winsize w = {24, 80, 0, 0};
    int fd = open("/dev/tty", O_RDONLY);
    if (fd >= 0) { ioctl(fd, TIOCGWINSZ, &w); close(fd); }
    int cols = w.ws_col > 0 ? w.ws_col : 80;
    int rows = w.ws_row > 0 ? w.ws_row - 1 : 23;

    /* dot resolution */
    int dw = cols * 2;
    int dh = rows * 4;
    float cx = dw / 2.0f;
    float cy = dh / 2.0f;
    float rx = dw / 2.0f - 2;
    float ry = dh / 2.0f - 2;
    float thickness = 1.5f;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            unsigned char bits = 0;
            for (int dr = 0; dr < 4; dr++) {
                for (int dc = 0; dc < 2; dc++) {
                    float dx = (c * 2 + dc - cx) / rx;
                    float dy = (r * 4 + dr - cy) / ry;
                    float dist = sqrtf(dx*dx + dy*dy);
                    if (fabsf(dist - 1.0f) < thickness / fminf(rx, ry))
                        bits |= (1 << dot_bit[dr][dc]);
                }
            }
            print_braille(bits);
        }
        printf("\n");
    }
    return 0;
}
