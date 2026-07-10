/**
 * @file graph.h
 * @brief Directed graph data structure for diagram representation.
 *
 * All parsers (mermaid, plantuml, drawio) produce a Graph as output.
 * Layout and render modules consume it. This is the shared contract
 * between pipeline stages.
 */
#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>

/** Maximum nodes in a single graph. */
constexpr int GRAPH_MAX_NODES = 64;

/** Layout direction. */
enum class direction { TD, LR };

/** A node in the graph with optional label and computed position. */
struct node {
  std::string id;    /**< Unique identifier (e.g. "A"). */
  std::string label; /**< Display label (e.g. "Start"). Defaults to id. */
  int layer = 0;     /**< Assigned layer after topological sort. */
  int order = 0;     /**< Position within layer. */
  float x = 0;       /**< Computed x in dot space. */
  float y = 0;       /**< Computed y in dot space. */
  float w = 0;       /**< Computed width in dot space. */
  float h = 0;       /**< Computed height in dot space. */
};

/** A directed edge between two nodes. */
struct edge {
  int from = 0; /**< Index into Graph::nodes. */
  int to = 0;   /**< Index into Graph::nodes. */
};

/** Directed graph produced by parsers, consumed by layout/render. */
struct graph {
  direction dir = direction::TD; /**< Layout direction. */
  std::vector<node> nodes;       /**< All nodes. */
  std::vector<edge> edges;       /**< All edges. */

  /**
   * Find node index by id. Returns -1 if not found.
   * @param id  node identifier to search for
   */
  int find_node(const std::string& id) const {
    for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
      if (nodes[i].id == id) {
        return i;
      }
    }
    return -1;
  }

  /**
   * Find or create a node by id. Returns its index.
   * @param id  node identifier
   */
  int get_or_add(const std::string& id) {
    int idx = find_node(id);
    if (idx >= 0) {
      return idx;
    }
    nodes.push_back({id, id});
    return static_cast<int>(nodes.size()) - 1;
  }
};

#endif
