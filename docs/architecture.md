# Architecture (C4)

## Level 1 — System Context

```mermaid
C4Context
    Person(user, "User", "Developer in a terminal")
    System(mtui, "mermaid-tui", "Renders markdown and mermaid diagrams in the terminal")
    System_Ext(fs, "Filesystem", "Markdown files")
    System_Ext(stdin, "stdin", "Piped input")

    Rel(user, mtui, "Opens file or pipes input")
    Rel(mtui, fs, "Reads .md files")
    Rel(stdin, mtui, "cat file.md |")
```

---

## Level 2 — Container

```mermaid
C4Container
    Person(user, "User")

    Container(parser, "Parser", "C", "Parses markdown and extracts mermaid blocks")
    Container(layout, "Layout Engine", "C", "Builds virtual canvas with float coordinates")
    Container(renderer, "Renderer", "C", "Projects virtual canvas onto terminal viewport")
    Container(tui, "TUI / Viewport", "C", "Handles input, scroll, zoom, renderer switching")

    Rel(user, tui, "Keyboard input")
    Rel(tui, parser, "Pass raw markdown")
    Rel(parser, layout, "Nodes, edges, text blocks")
    Rel(layout, renderer, "Virtual canvas (float coords)")
    Rel(renderer, tui, "Character buffer")
```

---

## Level 3 — Component: Renderer

```mermaid
C4Component
    Container_Boundary(renderer, "Renderer") {
        Component(ascii, "ASCII Renderer", "C", "Box-drawing chars: ┌─┐│└┘►")
        Component(block, "Block Renderer", "C", "Half-block chars: ▀▄█▌▐")
        Component(braille, "Braille Renderer", "C", "Braille dots: ⣿ (2x4 per cell, high res)")
        Component(viewport, "Viewport Projector", "C", "Scales float coords to terminal cols/rows, handles zoom + scroll")
    }

    Rel(viewport, ascii, "Render at scale")
    Rel(viewport, block, "Render at scale")
    Rel(viewport, braille, "Render at scale")
```

---

## Level 3 — Component: Layout Engine

```mermaid
C4Component
    Container_Boundary(layout, "Layout Engine") {
        Component(graph, "Graph Builder", "C", "Nodes + directed edges from parsed mermaid")
        Component(topo, "Topological Sort", "C", "Layer assignment for LR/TD layout")
        Component(canvas, "Virtual Canvas", "C", "Float coordinate space (0.0–1.0 normalized)")
        Component(router, "Edge Router", "C", "Routes edges between nodes avoiding overlaps")
    }

    Rel(graph, topo, "Node list + edges")
    Rel(topo, canvas, "Layered positions")
    Rel(canvas, router, "Node bounding boxes")
    Rel(router, canvas, "Edge paths as float polylines")
```

---

## Key design decisions

| Decision | Choice | Reason |
|----------|--------|--------|
| Coordinate space | Normalized floats (0.0–1.0) | Zoom/scale independent of terminal size |
| Renderer selection | Runtime switchable (a/b/c key) | Compare quality interactively |
| Terminal size | `ioctl(TIOCGWINSZ)` on resize | Reflow on SIGWINCH |
| Zoom | Scale factor applied in viewport projector | Virtual canvas unchanged |
| Scroll | Offset in viewport projector | Virtual canvas unchanged |
| Language | C | Consistent with tasks tool, no runtime deps |
