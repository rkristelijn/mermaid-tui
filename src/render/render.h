/**
 * @file render.h
 * @brief Render a laid-out graph to output via pluggable backends.
 *
 * Two extension points:
 * - draw_backend: how shapes are drawn (braille canvas, box-drawing, etc.)
 * - output_backend: how the result is flushed to stdout
 *
 * Default backends use the braille canvas. Swap them to get box-drawing
 * characters, plain ASCII, or any future output format.
 */
#ifndef RENDER_H
#define RENDER_H

#include "graph/graph.h"

/**
 * Drawing primitives — how shapes are drawn.
 * Each function pointer maps to a canvas-like operation.
 */
struct draw_backend {
  void (*init)(int cols, int rows);                        /**< Init with terminal size. */
  void (*clear)(void);                                     /**< Clear drawing surface. */
  void (*line)(float x0, float y0, float x1, float y1);    /**< Draw a line. */
  void (*rect)(float x, float y, float w, float h);        /**< Draw a rectangle. */
  void (*ellipse)(float cx, float cy, float rx, float ry); /**< Draw an ellipse. */
  void (*flush)(void);                                     /**< Output the result. */
  int (*width)(void);                                      /**< Dot-space width. */
  int (*height)(void);                                     /**< Dot-space height. */
};

/** Braille canvas backend (default). */
draw_backend make_braille_backend();

/** ASCII canvas backend ('*' / ' '). */
draw_backend make_ascii_backend();

/** Box-drawing backend (┌─┐│└┘). */
draw_backend make_box_backend();

/**
 * Render graph using the given drawing backend.
 * @param g    graph with computed positions (after layout_graph)
 * @param db   drawing backend to use
 */
void render_graph(const graph& g, const draw_backend& db);

#endif
