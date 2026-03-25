#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(void) {
    struct winsize w = {24, 80, 0, 0};
    int fd = open("/dev/tty", O_RDONLY);
    if (fd >= 0) { ioctl(fd, TIOCGWINSZ, &w); close(fd); }
    int cols = w.ws_col > 0 ? w.ws_col : 80;
    int rows = w.ws_row > 0 ? w.ws_row - 1 : 23;

    /* terminal cells are ~2x taller than wide, compensate */
    float cx = cols / 2.0f;
    float cy = rows / 2.0f;
    float rx = cols / 2.0f - 2;
    float ry = rows / 2.0f - 1;
    float threshold = 1.2f;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            float dx = (c - cx) / rx;
            float dy = (r - cy) / ry;
            float dist = sqrtf(dx*dx + dy*dy);
            if (fabsf(dist - 1.0f) < threshold / fminf(rx, ry * 2))
                printf("*");
            else
                printf(" ");
        }
        printf("\n");
    }
    return 0;
}
