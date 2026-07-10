/**
 * @file main.cpp
 * @brief Entry point — reads mermaid from stdin or file, renders to terminal.
 *
 * Pipeline: stdin/file → parse → layout → render → output.
 * Usage:
 *   echo 'graph TD; A-->B-->C' | ./mermaid-tui
 *   ./mermaid-tui diagram.mmd
 *   ./mermaid-tui --ascii diagram.mmd
 *   ./mermaid-tui --box diagram.mmd
 */
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "layout/layout.h"
#include "parser/parser.h"
#include "render/render.h"

/** Get terminal dimensions, fallback to 80x24. */
static void get_term_size(int& cols, int& rows) {
  struct winsize w = {24, 80, 0, 0};
  int fd = open("/dev/tty", O_RDONLY);
  if (fd >= 0) {
    ioctl(fd, TIOCGWINSZ, &w);
    close(fd);
  }
  cols = w.ws_col > 0 ? w.ws_col : 80;
  rows = w.ws_row > 0 ? w.ws_row - 1 : 23;
}

/** Parsed command-line options. */
struct cli_opts {
  const char* file_path;    /**< Input file, or nullptr for stdin. */
  const char* backend_name; /**< Render backend name. */
};

/** Parse argv into cli_opts. */
static cli_opts parse_args(int argc, const char* argv[]) {
  cli_opts opts = {nullptr, "braille"};
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--ascii") == 0) {
      opts.backend_name = "ascii";
    } else if (strcmp(argv[i], "--box") == 0) {
      opts.backend_name = "box";
    } else if (argv[i][0] != '-') {
      opts.file_path = argv[i];
    }
  }
  return opts;
}

/** Read mermaid input from file or stdin. Returns empty string on error. */
static std::string read_input(const char* path) {
  std::ostringstream buf;
  if (path != nullptr) {
    std::ifstream file(path);
    if (!file) {
      fprintf(stderr, "error: cannot open %s\n", path);
      return "";
    }
    buf << file.rdbuf();
  } else {
    buf << std::cin.rdbuf();
  }
  return buf.str();
}

/** Select draw backend by name. */
static draw_backend select_backend(const char* name) {
  if (strcmp(name, "ascii") == 0) {
    return make_ascii_backend();
  }
  if (strcmp(name, "box") == 0) {
    return make_box_backend();
  }
  return make_braille_backend();
}

/** Entry point — parse input, select backend, render graph. */
int main(int argc, const char* argv[]) {
  cli_opts opts = parse_args(argc, argv);

  std::string input = read_input(opts.file_path);
  if (input.empty()) {
    fprintf(stderr, "usage: mermaid-tui [--ascii|--box] [file.mmd]\n");
    return 1;
  }

  graph g = parse_mermaid(input);
  if (g.nodes.empty()) {
    fprintf(stderr, "error: no nodes parsed\n");
    return 1;
  }

  int cols = 0;
  int rows = 0;
  get_term_size(cols, rows);

  draw_backend db = select_backend(opts.backend_name);
  db.init(cols, rows);
  layout_graph(g, db.width(), db.height());
  render_graph(g, db);
  db.flush();

  return 0;
}
