/**
 * @file layout.cpp
 * @brief Topological sort layout for directed graphs.
 *
 * Assigns layers via longest-path from roots, then distributes nodes
 * evenly within each layer. Supports TD (top-down) and LR (left-right).
 */
#include "layout/layout.h"

#include <algorithm>
#include <vector>

/** Node box size as fraction of available space. */
static constexpr float NODE_W_FRAC = 0.6f;
static constexpr float NODE_H_FRAC = 0.5f;

/**
 * Compute in-degree for each node.
 * @return vector of in-degrees indexed by node id
 */
static std::vector<int> compute_in_degree(const graph& g) {
  std::vector<int> in_deg(g.nodes.size(), 0);
  for (const auto& e : g.edges) {
    in_deg[e.to]++;
  }
  return in_deg;
}

/**
 * Propagate layer from a node along its outgoing edges.
 * @return highest layer assigned during propagation
 */
static int propagate_layer(graph& g, int cur, std::vector<int>& in_deg, std::vector<int>& queue) {
  int max_layer = 0;
  for (const auto& e : g.edges) {
    if (e.from != cur) {
      continue;
    }
    int new_layer = g.nodes[cur].layer + 1;
    if (new_layer > g.nodes[e.to].layer) {
      g.nodes[e.to].layer = new_layer;
    }
    if (new_layer > max_layer) {
      max_layer = new_layer;
    }
    in_deg[e.to]--;
    if (in_deg[e.to] == 0) {
      queue.push_back(e.to);
    }
  }
  return max_layer;
}

/**
 * Assign layers using longest incoming path (BFS-like).
 * Roots (no incoming edges) get layer 0.
 */
static int assign_layers(graph& g) {
  auto in_deg = compute_in_degree(g);

  /* start from roots */
  std::vector<int> queue;
  for (int i = 0; i < static_cast<int>(g.nodes.size()); i++) {
    if (in_deg[i] == 0) {
      g.nodes[i].layer = 0;
      queue.push_back(i);
    }
  }

  /* propagate layers along edges */
  int max_layer = 0;
  for (size_t qi = 0; qi < queue.size(); qi++) {
    int layer = propagate_layer(g, queue[qi], in_deg, queue);
    if (layer > max_layer) {
      max_layer = layer;
    }
  }
  return max_layer;
}

void layout_graph(graph& g, int dw, int dh) {
  if (g.nodes.empty()) {
    return;
  }

  int max_layer = assign_layers(g);
  int layer_count = max_layer + 1;

  /* count nodes per layer for spacing */
  std::vector<int> layer_sizes(layer_count, 0);
  for (auto& nd : g.nodes) {
    nd.order = layer_sizes[nd.layer]++;
  }

  /* compute positions per node */
  for (auto& nd : g.nodes) {
    int count = layer_sizes[nd.layer];
    float cell_w = 0;
    float cell_h = 0;

    if (g.dir == direction::TD) {
      /* TD: layers go top-to-bottom, nodes spread left-to-right */
      cell_h = static_cast<float>(dh) / layer_count;
      cell_w = static_cast<float>(dw) / count;
      nd.w = cell_w * NODE_W_FRAC;
      nd.h = cell_h * NODE_H_FRAC;
      nd.x = cell_w * nd.order + (cell_w - nd.w) / 2;
      nd.y = cell_h * nd.layer + (cell_h - nd.h) / 2;
    } else {
      /* LR: layers go left-to-right, nodes spread top-to-bottom */
      cell_w = static_cast<float>(dw) / layer_count;
      cell_h = static_cast<float>(dh) / count;
      nd.w = cell_w * NODE_W_FRAC;
      nd.h = cell_h * NODE_H_FRAC;
      nd.x = cell_w * nd.layer + (cell_w - nd.w) / 2;
      nd.y = cell_h * nd.order + (cell_h - nd.h) / 2;
    }
  }
}
