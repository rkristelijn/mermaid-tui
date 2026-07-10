# Visualization Research

How existing tools render mermaid diagrams in the terminal.

## Characters used

### Unicode mode (default)

```text
┌ ┐ └ ┘ │ ─   box corners and lines
├ ┤ ┬ ┴ ┼     T-junctions and cross
► ◄ ▲ ▼       solid arrowheads
┈             dotted line (sequence diagrams, dashed arrows)
```

### ASCII fallback (`--ascii` flag)

```text
+ - |          box corners and lines
> < ^ v        arrowheads
. or <.        dotted arrows
```

---

## Rendering strategy (mermaid-ascii)

Source: <https://github.com/AlexanderGrooff/mermaid-ascii>

### 1. Parse mermaid into graph nodes + edges

The mermaid syntax is parsed into a list of nodes and directed edges with optional labels.

### 2. Build a virtual grid

A coordinate grid is constructed where:

- Each node occupies **3 grid columns × 3 grid rows** (left border, content, right border)
- Each gap between nodes occupies **1 grid column/row** for routing edges
- Grid coordinates are logical, not pixel/character coordinates

```text
0      1      2  3  4      5      6
|      |      |  |  |      |      |
v      v      v  v  v      v      v

+-------------+     +-------------+
|             |     |             |
|  Some text  |---  |  Some text  |
|             |  |  |             |
+-------------+  |  +-------------+
                 |
                 |
+-------------+  |  +-------------+
|             |  |  |             |
|  Some text  |  -->|  Some text  |
|             |     |             |
+-------------+     +-------------+
```

### 3. Path edges through the grid

Edges are routed as paths through the grid coordinates, e.g. `[(2,1), (3,1), (3,5), (4,5)]`. This avoids overlapping nodes. Edges travel horizontally or vertically — no diagonal lines.

### 4. Convert grid to drawing coordinates

Grid coordinates are mapped to character positions on the canvas. Each grid unit has a configurable size:

- `-x` flag: horizontal spacing between nodes (default 5 chars)
- `-y` flag: vertical spacing between nodes (default 5 chars)
- `-p` flag: padding inside node box (default 1 char)

### 5. Render to stdout

The canvas is a 2D character array. Nodes and edges are drawn into it, then printed line by line.

---

## Terminal size awareness

mermaid-ascii does **not** automatically adapt to terminal width/height. The output width depends entirely on the number of nodes and the `-x` spacing. There is an open TODO in the repo:

> "Prevent rendering more than X characters wide (like default 80 for terminal width)"

So terminal-aware rendering is **not yet implemented** in mermaid-ascii.

For a proper TUI tool, this needs to be handled explicitly:

- Read terminal size with `ioctl(TIOCGWINSZ)` (C/POSIX) or `term.GetSize()` (Go)
- Either scale down spacing, truncate, or add horizontal scrolling

---

## Layout algorithms

mermaid-ascii uses a simple **topological sort + layer assignment**:

- Nodes are placed in layers (columns for LR, rows for TD)
- No advanced graph layout (no Sugiyama, no force-directed)
- Works well for DAGs, struggles with cycles and dense graphs

For sequence diagrams, layout is simpler: participants are columns, messages are rows in order.

---

## Supported diagram types (mermaid-ascii v1.1.0)

| Type | Support |
|------|---------|
| Flowchart LR/TD | ✅ |
| Sequence diagram | ✅ |
| Subgraphs | ✅ |
| Labeled edges | ✅ |
| classDef colors | ✅ |
| Class diagrams | ❌ |
| State diagrams | ❌ |
| Gantt | ❌ |
| Pie charts | ❌ |

---

## Implications for mermaid-tui

- Reusing mermaid-ascii as a library (it's Go) is feasible
- Terminal size detection needs to be added on top
- Scrolling is needed for diagrams wider than terminal width
- A virtual canvas approach (draw to buffer, then render viewport) is the right architecture
- For markdown rendering, glow's rendering library ([glamour](https://github.com/charmbracelet/glamour)) can be reused
