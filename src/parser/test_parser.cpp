/**
 * @file test_parser.cpp
 * @brief Unit tests for the mermaid flowchart parser.
 */
#include <cassert>
#include <cstdio>

#include "parser/parser.h"

/** Minimal test harness macros. */
#define TEST(name)           \
  static void name();        \
  static struct name##_reg { \
    name##_reg() { name(); } \
  } name##_instance;         \
  static void name()

// NOLINTBEGIN(bugprone-macro-parentheses)
#define CHECK(expr)                                                   \
  do {                                                                \
    if (!(expr)) {                                                    \
      fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #expr); \
      assert(false);                                                  \
    }                                                                 \
  } while (0)
// NOLINTEND(bugprone-macro-parentheses)

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST(parse_simple_td) {
  auto g = parse_mermaid("graph TD\n  A --> B\n");
  CHECK (g.dir == direction::TD)
    ;
  CHECK (g.nodes.size() == 2)
    ;
  CHECK (g.edges.size() == 1)
    ;
  CHECK (g.nodes[0].id == "A")
    ;
  CHECK (g.nodes[1].id == "B")
    ;
  CHECK (g.edges[0].from == 0)
    ;
  CHECK (g.edges[0].to == 1)
    ;
  printf("  [pass] parse_simple_td\n");
}

TEST(parse_lr_direction) {
  auto g = parse_mermaid("graph LR\n  X --> Y\n");
  CHECK (g.dir == direction::LR)
    ;
  printf("  [pass] parse_lr_direction\n");
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST(parse_chain) {
  auto g = parse_mermaid("graph TD\n  A-->B-->C\n");
  CHECK (g.nodes.size() == 3)
    ;
  CHECK (g.edges.size() == 2)
    ;
  CHECK (g.edges[0].from == 0)
    ;
  CHECK (g.edges[0].to == 1)
    ;
  CHECK (g.edges[1].from == 1)
    ;
  CHECK (g.edges[1].to == 2)
    ;
  printf("  [pass] parse_chain\n");
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST(parse_labels) {
  auto g = parse_mermaid("graph TD\n  A[Start] --> B[End]\n");
  CHECK (g.nodes[0].label == "Start")
    ;
  CHECK (g.nodes[1].label == "End")
    ;
  printf("  [pass] parse_labels\n");
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST(parse_semicolons) {
  auto g = parse_mermaid("graph TD; A-->B; B-->C");
  CHECK (g.nodes.size() == 3)
    ;
  CHECK (g.edges.size() == 2)
    ;
  printf("  [pass] parse_semicolons\n");
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST(parse_flowchart_keyword) {
  auto g = parse_mermaid("flowchart TD\n  A-->B\n");
  CHECK (g.nodes.size() == 2)
    ;
  CHECK (g.dir == direction::TD)
    ;
  printf("  [pass] parse_flowchart_keyword\n");
}

TEST(parse_empty_input) {
  auto g = parse_mermaid("");
  CHECK (g.nodes.empty())
    ;
  printf("  [pass] parse_empty_input\n");
}

TEST(parse_no_edges) {
  auto g = parse_mermaid("graph TD\n");
  CHECK (g.nodes.empty())
    ;
  printf("  [pass] parse_no_edges\n");
}

int main() {
  printf("test_parser: all tests passed\n");
  return 0;
}
