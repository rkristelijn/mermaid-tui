/**
 * @file parser.cpp
 * @brief Mermaid flowchart parser implementation.
 *
 * Parses a minimal mermaid subset: "graph TD/LR" header followed by
 * edge definitions like "A[Label] --> B[Label]". Chains (A-->B-->C)
 * are expanded into individual edges.
 *
 * @see docs/diagram-renderer-research.md — mermaid-ascii parser as reference
 */
#include "parser/parser.h"

#include <algorithm>
#include <sstream>

/** Remove leading/trailing whitespace from a string. */
static std::string trim(const std::string& str) {
  auto start = str.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) {
    return "";
  }
  auto end = str.find_last_not_of(" \t\r\n");
  return str.substr(start, end - start + 1);
}

/**
 * Parse a node token like "A", "A[Label]", or "A[Some text]".
 * Extracts id and optional label, adds to graph.
 * @return index of the node in graph
 */
static int parse_node_token(graph& g, const std::string& token) {
  auto bracket = token.find('[');
  if (bracket == std::string::npos) {
    return g.get_or_add(token);
  }
  /* extract id and label from "ID[Label]" */
  std::string id = token.substr(0, bracket);
  auto close = token.find(']', bracket);
  std::string label = token.substr(bracket + 1, close - bracket - 1);
  int idx = g.get_or_add(id);
  g.nodes[idx].label = label;
  return idx;
}

/**
 * Split a line on "-->" and create edges for each pair.
 * Handles chains: "A-->B-->C" becomes edges A→B and B→C.
 */
static void parse_edge_line(graph& g, const std::string& line) {
  std::string remaining = line;
  std::string arrow = "-->";
  std::vector<int> chain;

  while (true) {
    auto pos = remaining.find(arrow);
    std::string token;
    if (pos == std::string::npos) {
      token = trim(remaining);
      if (!token.empty()) {
        chain.push_back(parse_node_token(g, token));
      }
      break;
    }
    token = trim(remaining.substr(0, pos));
    if (!token.empty()) {
      chain.push_back(parse_node_token(g, token));
    }
    remaining = remaining.substr(pos + arrow.size());
  }

  /* create edges between consecutive nodes in chain */
  for (size_t i = 1; i < chain.size(); i++) {
    g.edges.push_back({chain[i - 1], chain[i]});
  }
}

/** Try to parse a mermaid header line. Returns true if valid header found. */
static bool try_parse_header(const std::string& line, direction& dir) {
  if (!line.starts_with("graph") && !line.starts_with("flowchart")) {
    return false;
  }
  if (line.find("LR") != std::string::npos) {
    dir = direction::LR;
  }
  return true;
}

graph parse_mermaid(const std::string& input) {
  graph g;

  /* replace semicolons with newlines for uniform parsing */
  std::string normalized = input;
  std::replace(normalized.begin(), normalized.end(), ';', '\n');

  std::istringstream stream(normalized);
  std::string line;
  bool header_found = false;

  while (std::getline(stream, line)) {
    line = trim(line);
    if (line.empty()) {
      continue;
    }

    /* first non-empty line must be "graph TD" or "graph LR" */
    if (!header_found) {
      header_found = try_parse_header(line, g.dir);
      continue;
    }

    /* skip lines without arrows — comments, subgraph, etc. */
    if (line.find("-->") == std::string::npos) {
      continue;
    }

    parse_edge_line(g, line);
  }

  return g;
}
