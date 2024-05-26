// framebf.h
#ifndef __FRAMEBF_H__
#define __FRAMEBF_H__

typedef unsigned long Bitmap;

long long get_rendered_pixels();
void reset_rendered_pixels();
void print_rendered_pixels();
void print_pixel_diff(unsigned long long start, char *message);
void initialize_frame_buffer(int physicalWidth, int physicalHeight,
                             int virtualWidth, int virtualHeight);
void draw_pixel_ARGB_32(int x, int y, unsigned int attr);
void draw_rect_ARGB_32(int x1, int y1, int x2, int y2, unsigned int attr,
                       int fill);
void draw_pixel(int x, int y, unsigned int attr);
void draw_image(int x, int y, int width, int height, const Bitmap *bitmap);
void draw_rect(int x1, int y1, int x2, int y2, unsigned int attr, int fill);
void draw_line(int x1, int y1, int x2, int y2, unsigned int attr,
               int lineWidth);
void draw_circle(int x0, int y0, int radius, unsigned char attr, int fill);
void draw_char(unsigned char ch, int x, int y, unsigned int attr, int zoom);
void draw_string(int x, int y, char *s, unsigned int attr, int zoom);
void move_rect(int x, int y, int width, int height, int xoff, int yoff,
               unsigned char attr);
void copy_rect(int srcX, int srcY, int destX, int destY, int srcWidth,
               int destWidth, int destHeight, const Bitmap *srcBitmap,
               const Bitmap *dest);
void copy_rect_alpha(int srcX, int srcY, int destX, int destY, int srcWidth,
                     int destWidth, int destHeight, const Bitmap *srcBitmap,
                     Bitmap *dest, unsigned int attr);
void draw_rect_from_bitmap(int x, int y, int width, int height,
                           const Bitmap *bitmap);
void draw_rect_from_bitmap_alpha(int x, int y, int width, int height,
                                 const Bitmap *bitmap, unsigned int attr);
void draw_transparent_image(int x, int y, int width, int height,
                            const Bitmap *bitmap);
void clear_frame_buffer(int width, int height);
#endif
