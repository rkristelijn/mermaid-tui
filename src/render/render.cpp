/**
 * @file render.cpp
 * @brief Render graph via pluggable draw backends.
 *
 * The render logic (node placement, edge routing) is backend-agnostic.
 * It calls draw_backend function pointers instead of canvas functions
 * directly, so any output format can be plugged in.
 *
 * @see render.h — draw_backend struct definition
 */
#include "render/render.h"

#include "canvas/canvas.h"

/* --- backend factories --- */

draw_backend make_braille_backend() {
  return {canvas_init,
          canvas_clear,
          canvas_line,
          canvas_rect,
          canvas_ellipse,
          canvas_print_braille,
          []() -> int { return canvas_dw; },
          []() -> int { return canvas_dh; }};
}

draw_backend make_ascii_backend() {
  return {canvas_init,
          canvas_clear,
          canvas_line,
          canvas_rect,
          canvas_ellipse,
          canvas_print_ascii,
          []() -> int { return canvas_dw; },
          []() -> int { return canvas_dh; }};
}

draw_backend make_box_backend() {
  /* TODO: implement box-drawing output (┌─┐│└┘) — falls back to braille */
  return make_braille_backend();
}

/* --- rendering logic (backend-agnostic) --- */

/** Draw a single edge as a line between node connection points. */
static void draw_edge(const graph& g, const edge& e, const draw_backend& db) {
  const node& from = g.nodes[e.from];
  const node& to = g.nodes[e.to];

  float x0 = 0;
  float y0 = 0;
  float x1 = 0;
  float y1 = 0;

  if (g.dir == direction::TD) {
    x0 = from.x + from.w / 2;
    y0 = from.y + from.h;
    x1 = to.x + to.w / 2;
    y1 = to.y;
  } else {
    x0 = from.x + from.w;
    y0 = from.y + from.h / 2;
    x1 = to.x;
    y1 = to.y + to.h / 2;
  }

  db.line(x0, y0, x1, y1);
}

void render_graph(const graph& g, const draw_backend& db) {
  for (const auto& nd : g.nodes) {
    db.rect(nd.x, nd.y, nd.w, nd.h);
  }
  for (const auto& e : g.edges) {
    draw_edge(g, e, db);
  }
}
