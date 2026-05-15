# Diagram Renderer Research

Research into existing tools, approaches, and own projects for rendering
diagrams (Mermaid, PlantUML, Draw.io, custom) in the terminal.

## What makes mermaid-tui unique

- **Braille rendering** (2×4 dots per cell = 8× resolution vs ASCII) — no other
  mermaid/diagram terminal renderer does this
- **Multi-format** — goal is Mermaid + PlantUML + Draw.io + custom DSL
- **Pure C, zero dependencies** — works over SSH, in tmux, anywhere with Unicode
- **Integration target** — llama-cli (backlog #032) needs mermaid rendering
  during markdown output in the REPL

## Own projects (~/git/hub/)

### ascii/ — Braille ASCII Generator (React + Node CLI)

- **Web app** (`src/App.js`) + **CLI** (`cli.js`) for text/image → braille
- Uses Jimp for image processing, canvas for text rendering
- Converts pixel grid to braille characters (same 2×4 dot encoding as mermaid-tui)
- Supports ANSI 256-color output, edge detection, posterize effects
- **Reusable logic**: `rgbToAnsi()`, braille dot mapping, threshold/contrast
- Path: `~/git/hub/ascii/`

### markdown-mermaid-mcp/ — MCP Server for Markdown & Mermaid

- Model Context Protocol server exposing markdown/mermaid knowledge as tools
- Contains structured docs for GitLab Flavored Markdown and Mermaid syntax
- References `~/git/hub/mermaid` (full mermaid-js clone)
- Path: `~/git/hub/markdown-mermaid-mcp/`

### mermaid/ — Full mermaid-js fork

- Complete clone of mermaid-js with all diagram parsers
- Useful as reference for mermaid syntax parsing rules
- Path: `~/git/hub/mermaid/`

### llama-cli — C++ CLI chat tool (needs mermaid rendering)

- Has markdown renderer (ADR-052) with streaming support
- Backlog #032: detect ` ```mermaid ` blocks, render as ASCII/braille
- Backlog #031: inline code rendering (done)
- mermaid-tui could be called as external renderer or linked as library
- Path: `~/git/hub/llama-cli/`

## Existing mermaid terminal renderers

| Project | Lang | Stars | Approach |
|---------|------|-------|----------|
| [mermaid-ascii](https://github.com/AlexanderGrooff/mermaid-ascii) | Go | 1.3k | Custom parser → Unicode box-drawing (┌─┐│└┘) |
| [mermaid-rs-renderer](https://github.com/1jehuang/mermaid-rs-renderer) | Rust | 1.2k | Native parser → SVG/PNG (not terminal) |
| [selkie](https://github.com/btucker/selkie) | Rust | 20 | Full parser, WASM, CLI + library |
| [rusty-mermaid](https://lib.rs/crates/rusty-mermaid) | Rust | — | 25 diagram types, 5 render backends |
| [mermaid-cli](https://github.com/mermaid-js/mermaid-cli) | JS | 4.4k | Puppeteer → image (slow, not terminal) |

**Key insight**: No braille mermaid renderer exists. mermaid-ascii is the only
pure terminal solution and uses box-drawing, not braille.

## PlantUML terminal rendering

- **PlantUML** itself has `-txt` (ASCII) and `-utxt` (Unicode) output modes
- No standalone terminal renderer — must run the Java app
- Could shell out to `plantuml -txt` and capture output

## Draw.io parsing

- Files are XML (`<mxGraphModel>`) with optional base64+zlib compression
- [drawpyo](https://github.com/MerrimanInd/drawpyo) (Python) — write .drawio
- [drawio-parser](https://pypi.org/project/drawio-parser/) (Python) — read .drawio
- Parsing in C: standard XML + zlib decompress + base64 decode

## ASCII graph layout tools

| Project | Lang | Notes |
|---------|------|-------|
| [Graph::Easy](https://metacpan.org/pod/Graph::Easy) | Perl | Gold standard for ASCII graph layout |
| [dot-to-ascii](https://github.com/ggerganov/dot-to-ascii) | PHP | Web wrapper around Graph::Easy |
| [asciidag](https://github.com/sambrightman/asciidag) | Python | DAG visualization |

## Braille/terminal graphics libraries

| Project | Lang | Notes |
|---------|------|-------|
| [drawille](https://github.com/asciimoo/drawille) | Python | Original braille-in-terminal lib |
| [chafa](https://github.com/hpjansson/chafa) | C | Most advanced terminal graphics (4.3k ★) |
| [ploot](https://lib.rs/crates/ploot) | Rust | Braille charts, zero deps |
| [plotext](https://github.com/piccolomo/plotext) | Python | matplotlib-like terminal plots |

## Graph layout algorithms (C/C++)

| Project | Notes |
|---------|-------|
| [igraph](https://igraph.org/c/) | C library with Sugiyama layout |
| [igraph-sugiyama](https://github.com/gml4gtk/igraph-sugiyama) | Standalone C Sugiyama |
| [dagre](https://github.com/dagrejs/dagre) | JS, hierarchical DAG layout (used by mermaid-js) |
| [BGL-sugiyama](https://github.com/lokimx88/BGL-sugiyama) | Boost Graph Library |

## Recommended approach

```text
input text ──► parser ──► graph {nodes, edges} ──► layout ──► canvas ──► braille
                │                                    │
          (adapter per       (topological sort +     │
           format)            layer assignment)      │
                                                     ▼
  Mermaid ──────┤                              existing canvas.h
  PlantUML ─────┤                              (line, rect, ellipse,
  Draw.io ──────┤                               dot, print_braille)
  Custom DSL ───┘
```

1. **Parser layer** — one adapter per format, all produce same graph struct
2. **Layout** — start with simple topological sort (like mermaid-ascii), upgrade
   to Sugiyama later if needed
3. **Render** — map nodes to `canvas_rect()` + text, edges to `canvas_line()`
4. **Integration** — llama-cli calls mermaid-tui as subprocess or links library

### Reference repos to study

- **mermaid-ascii** parser: `pkg/graph/` in the Go repo — cleanest mermaid parser
- **drawille** Python: understand the braille encoding (same as our canvas.h)
- **dagre** layout: `lib/layout.js` — the algorithm mermaid-js uses
- **chafa** C: terminal capability detection, color handling
