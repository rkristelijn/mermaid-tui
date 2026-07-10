/**
 * @file canvas.h
 * @brief Braille dot canvas API — drawing primitives and terminal renderers.
 *
 * Each terminal cell maps to a 2×4 braille dot block, giving 8× more
 * resolution than plain ASCII. All drawing functions operate in dot space
 * (canvas_dw × canvas_dh). Call canvas_init() before any other function.
 *
 * @see ../docs/renderer-decision.md
 * @see ../docs/architecture.md
 */
#ifndef CANVAS_H
#define CANVAS_H

#define CANVAS_MAXCOLS 400 /**< Maximum dot canvas width (cols × 2). */
#define CANVAS_MAXROWS 200 /**< Maximum dot canvas height (rows × 4). */

/**
 * Initialize canvas for given terminal dimensions.
 * Must be called before any drawing function.
 *
 * @param term_cols  terminal width in columns
 * @param term_rows  terminal height in rows
 */
void canvas_init(int term_cols, int term_rows);

/** Clear all dots. */
void canvas_clear(void);

/**
 * Set a single dot at dot-space coordinates.
 * Silently ignored if out of bounds.
 *
 * @param dr  dot row (0 .. canvas_dh-1)
 * @param dc  dot column (0 .. canvas_dw-1)
 */
void canvas_dot(int dr, int dc);

/**
 * Draw a line from (x0,y0) to (x1,y1) in dot space.
 * Samples at 2× the Euclidean length to avoid gaps.
 *
 * @param x0  start x
 * @param y0  start y
 * @param x1  end x
 * @param y1  end y
 */
void canvas_line(float x0, float y0, float x1, float y1);

/**
 * Draw an ellipse outline in dot space.
 * For a circle, pass rx == ry.
 *
 * @param cx  center x
 * @param cy  center y
 * @param rx  horizontal radius
 * @param ry  vertical radius
 */
void canvas_ellipse(float cx, float cy, float rx, float ry);

/**
 * Draw a rectangle outline in dot space.
 *
 * @param x  top-left x
 * @param y  top-left y
 * @param w  width
 * @param h  height
 */
void canvas_rect(float x, float y, float w, float h);

/**
 * Render dot canvas to stdout as Unicode braille (U+2800–U+28FF).
 * Each terminal cell is encoded as a 3-byte UTF-8 codepoint.
 */
void canvas_print_braille(void);

/**
 * Render dot canvas as plain ASCII ('*' / ' ').
 * Fallback for terminals without braille font support.
 */
void canvas_print_ascii(void);

/** Dot canvas width in dots (= term_cols × 2). */
extern int canvas_dw;

/** Dot canvas height in dots (= term_rows × 4). */
extern int canvas_dh;

#endif
