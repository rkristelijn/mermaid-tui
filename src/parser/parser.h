/**
 * @file parser.h
 * @brief Mermaid flowchart parser — text to graph.
 *
 * Supported subset:
 *   graph TD | graph LR
 *   A --> B
 *   A[Label] --> B[Label]
 *   Semicolons as line separators
 */
#ifndef PARSER_H
#define PARSER_H

#include <string>

#include "graph/graph.h"

/**
 * Parse mermaid flowchart text into a graph.
 * @param input  mermaid source (e.g. "graph TD; A-->B-->C")
 * @return populated graph, or empty graph on parse error
 */
graph parse_mermaid(const std::string& input);

#endif
