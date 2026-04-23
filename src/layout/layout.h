/**
 * @file layout.h
 * @brief Graph layout — assign positions to nodes in dot space.
 *
 * Uses topological sort to assign layers, then spaces nodes evenly
 * within each layer. Positions are in dot-space coordinates ready
 * for the canvas.
 */
#ifndef LAYOUT_H
#define LAYOUT_H

#include "graph/graph.h"

/**
 * Compute node positions for the given canvas dimensions.
 * Modifies node x, y, w, h fields in place.
 *
 * @param g   graph with nodes and edges
 * @param dw  canvas width in dots
 * @param dh  canvas height in dots
 */
void layout_graph(graph& g, int dw, int dh);

#endif
