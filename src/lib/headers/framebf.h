//framebf.h
#ifndef __FRAMEBF_H__
#define __FRAMEBF_H__

void framebf_init(int physicalWidth, int physicalHeight, int virtualWidth,
                  int virtualHeight);
void drawPixelARGB32(int x, int y, unsigned int attr);
void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr,
                    int fill);
void drawPixel(int x, int y, unsigned int attr);
void drawRect(int x1, int y1, int x2, int y2, unsigned int attr, int fill);
void drawLine(int x1, int y1, int x2, int y2, unsigned char attr);
void drawCircle(int x0, int y0, int radius, unsigned char attr, int fill);
void drawChar(unsigned char ch, int x, int y, unsigned int attr, int zoom);
void drawString(int x, int y, char *s, unsigned int attr, int zoom);
void moveRect(int x, int y, int width, int height, int xoff, int yoff,
              unsigned char attr);

#endif