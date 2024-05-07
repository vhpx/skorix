//This is font.h
#ifndef FONT_H
#define FONT_H

enum {
    FONT_WIDTH     = 8,  // Width in pixels
    FONT_HEIGHT    = 8,  // Height in pixel
    FONT_BPG       = 8,  // Bytes per character glyph
    FONT_BPL       = 1,  // Bytes per line
    FONT_NUMGLYPHS = 224 // Total number of characters in the font
};

extern unsigned char font[FONT_NUMGLYPHS][FONT_BPG];

void drawChar(unsigned char ch, int x, int y, unsigned int attr, int zoom);
void drawString(int x, int y, char *str, unsigned int attr, int zoom);
void displayFont(int screen_width, int screen_height, int image_width, int image_height);


#endif
