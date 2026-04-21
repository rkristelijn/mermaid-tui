#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "canvas.h"

int main(void) {
  struct winsize w = {24, 80, 0, 0};
  int fd = open("/dev/tty", O_RDONLY);
  if (fd >= 0) {
    ioctl(fd, TIOCGWINSZ, &w);
    close(fd);
  }
  int cols = w.ws_col > 0 ? w.ws_col : 80;
  int rows = w.ws_row > 0 ? w.ws_row - 1 : 23;

  canvas_init(cols, rows);

  float dw = canvas_dw, dh = canvas_dh;

  /* oval */
  canvas_ellipse(dw * 0.25f, dh * 0.25f, dw * 0.2f, dh * 0.1f);
  /* circle */
  canvas_ellipse(dw * 0.75f, dh * 0.25f, dw * 0.15f, dh * 0.2f);
  /* rect */
  canvas_rect(dw * 0.05f, dh * 0.55f, dw * 0.35f, dh * 0.35f);
  /* triangle */
  canvas_line(dw * 0.75f, dh * 0.55f, dw * 0.6f, dh * 0.9f);
  canvas_line(dw * 0.6f, dh * 0.9f, dw * 0.9f, dh * 0.9f);
  canvas_line(dw * 0.9f, dh * 0.9f, dw * 0.75f, dh * 0.55f);

  canvas_print_braille();
  return 0;
}
