// framebf.h
#ifndef __FRAMEBF_H__
#define __FRAMEBF_H__

void framebf_init(int physicalWidth, int physicalHeight, int virtualWidth,
                  int virtualHeight);
void draw_pixel_ARGB_32(int x, int y, unsigned int attr);
void draw_rect_ARGB_32(int x1, int y1, int x2, int y2, unsigned int attr,
                       int fill);
void draw_pixel(int x, int y, unsigned int attr);
void draw_rect(int x1, int y1, int x2, int y2, unsigned int attr, int fill);
void draw_line(int x1, int y1, int x2, int y2, unsigned char attr);
void draw_circle(int x0, int y0, int radius, unsigned char attr, int fill);
void draw_char(unsigned char ch, int x, int y, unsigned int attr, int zoom);
void draw_string(int x, int y, char *s, unsigned int attr, int zoom);
void move_rect(int x, int y, int width, int height, int xoff, int yoff,
               unsigned char attr);

#endif