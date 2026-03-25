# mermaid-tui

A terminal viewer for Markdown and Mermaid diagrams.

## Idea

Render `.md` files in the terminal with:
- Formatted markdown (headings, lists, tables, code blocks)
- Mermaid diagrams rendered as ASCII/Unicode art inline

## Goals

- Fast, keyboard-driven navigation
- No browser needed
- Works over SSH
- Pipe-friendly: `cat README.md | mermaid-tui`

## Status

Early idea phase. See `docs/design.md` for architecture thoughts.
