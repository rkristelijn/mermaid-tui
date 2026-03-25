#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(void) {
    struct winsize w = {24, 80, 0, 0};
    int fd = open("/dev/tty", O_RDONLY);
    if (fd >= 0) { ioctl(fd, TIOCGWINSZ, &w); close(fd); }
    int cols = w.ws_col > 0 ? w.ws_col : 80;
    int rows = w.ws_row > 0 ? w.ws_row - 1 : 23;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int top    = (r == 0);
            int bottom = (r == rows - 1);
            int left   = (c == 0);
            int right  = (c == cols - 1);

            if      (top    && left)  printf("┌");
            else if (top    && right) printf("┐");
            else if (bottom && left)  printf("└");
            else if (bottom && right) printf("┘");
            else if (top    || bottom) printf("─");
            else if (left   || right)  printf("│");
            else printf(" ");
        }
        printf("\n");
    }
    return 0;
}
