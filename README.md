# mermaid-tui

> **Status: early setup phase** — not usable yet. Architecture and tooling are being established.

Render Markdown and Mermaid diagrams in the terminal — no browser, no dependencies.

## What it does

- Renders Mermaid diagrams (flowcharts, sequence diagrams) as high-resolution braille art
- Displays Markdown inline in the terminal
- Works over SSH, in tmux, anywhere with a Unicode terminal
- Pipe-friendly: `cat README.md | mermaid-tui`

## Status

Early development. See [TODO.md](TODO.md) for what's planned.

## Usage

```bash
make
./mermaid-tui file.md
cat diagram.mmd | ./mermaid-tui
```

Keyboard:

- `q` — quit
- `↑↓` — scroll
- `+/-` — zoom

## See also

- [CONTRIBUTING.md](CONTRIBUTING.md) — how to build, test, and contribute
- [docs/architecture.md](docs/architecture.md) — C4 design
- [docs/renderer-decision.md](docs/renderer-decision.md) — why braille
