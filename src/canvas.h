#ifndef CANVAS_H
#define CANVAS_H

#define CANVAS_MAXCOLS 400
#define CANVAS_MAXROWS 200

/* Dot canvas: 2 dots wide x 4 dots tall per terminal cell.
 * Coordinates are in dot space: dc = col*2+[0,1], dr = row*4+[0,3] */
void canvas_init(int term_cols, int term_rows);
void canvas_clear(void);
void canvas_dot(int dr, int dc);
void canvas_line(float x0, float y0, float x1, float y1);
void canvas_ellipse(float cx, float cy, float rx, float ry);
void canvas_rect(float x, float y, float w, float h);
void canvas_print_braille(void);
void canvas_print_ascii(void);

/* dot dimensions */
extern int canvas_dw;
extern int canvas_dh;

#endif
