#include "img.h"
#include "img_src.h"
#include "../lib/headers/framebf.h"

// starting offsets
int x_offset = 0;
int y_offset = 0;
const int scroll_step = 20; 

void displayImage() {
    framebf_init(SCREEN_WIDTH, SCREEN_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT);

    if (x_offset < 0) x_offset = 0;
    if (y_offset < 0) y_offset = 0;
    if (x_offset > IMAGE_WIDTH - SCREEN_WIDTH) x_offset = IMAGE_WIDTH - SCREEN_WIDTH;
    if (y_offset > IMAGE_HEIGHT - SCREEN_HEIGHT) y_offset = IMAGE_HEIGHT - SCREEN_HEIGHT;

    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        int imgY = y + y_offset;
        if (imgY >= IMAGE_HEIGHT) break;
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            int imgX = x + x_offset;
            if (imgX >= IMAGE_WIDTH) break;

            unsigned long pixel = epd_bitmap_image[imgY * IMAGE_WIDTH + imgX];
            drawPixelARGB32(x, y, pixel);
        }
    }
}

void scrollImage(char key) {
    if (key == 'w') y_offset -= scroll_step;  // scroll up
    if (key == 's') y_offset += scroll_step;  // scroll down
    if (key == 'a') x_offset -= scroll_step;  // scroll left
    if (key == 'd') x_offset += scroll_step;  // scroll right

    // redraw image
    displayImage();
}
