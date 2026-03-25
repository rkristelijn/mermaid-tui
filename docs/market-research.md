# Market Research

## Markdown TUI viewers

| Tool | Language | Strengths | Weaknesses |
|------|----------|-----------|------------|
| [glow](https://github.com/charmbracelet/glow) | Go | Most popular, beautiful rendering, built-in file browser, pipe support | No mermaid support |
| [mdcat](https://mdcat.frankchan.dev/) | Rust | Inline images (iTerm2/kitty/WezTerm) | No TUI navigation, no mermaid |
| [md-tui](https://lib.rs/crates/md-tui) | Rust | Full TUI, keyboard navigation | No mermaid |

## Mermaid → terminal renderers

| Tool | Language | Strengths | Weaknesses |
|------|----------|-----------|------------|
| [mermaid-ascii](https://github.com/AlexanderGrooff/mermaid-ascii) | Go | Unicode box-drawing, CLI-ready | Mermaid only, no markdown |
| [mermaid-ascii-diagrams](https://pypi.org/project/mermaid-ascii-diagrams/) | Python | Extracts mermaid from markdown, renders inline | Python dependency, limited diagram types |
| [beautiful-mermaid](https://cssscript.com/beautiful-mermaid-svg-ascii/) | JS | SVG and ASCII output | JS library, no CLI |

## Existing combinations

- [glow + mermaid-ascii wrapper](https://gist.github.com/olavocarvalho/749053cb283642044064b93f183a056b) — shell script combining glow and mermaid-ascii. Proof of concept, unmaintained.
- [MermaidTUI (HN)](https://news.ycombinator.com/item?id=46734292) — deterministic Unicode/ASCII in terminal, recent Show HN post.

## Conclusion

No mature integrated tool exists that renders both markdown and mermaid in a single TUI. The building blocks are there (glow + mermaid-ascii). The gap is:

- Single binary
- Markdown rendering + mermaid inline
- Keyboard navigation
- Works over SSH, no browser needed
- Pipe-friendly

## Language recommendation

| Option | Pro | Con |
|--------|-----|-----|
| Go | glow and mermaid-ascii are already Go, reuse possible | less low-level control |
| Rust | md-tui is Rust, great TUI libs (ratatui) | steeper learning curve |
| C | like the tasks tool, fast, no runtime | lots to build from scratch |

Go seems most logical given the ecosystem (glow, mermaid-ascii, bubbletea).
