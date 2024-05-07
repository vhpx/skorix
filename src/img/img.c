// this file is img.c
#include "img.h"
#include "../lib/headers/framebf.h"
#include "img_src.h"


// starting offsets
int x_offset = 0;
int y_offset = 0;
const int scroll_step = 20; // step size

void clearFramebuffer(int width, int height) {
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      drawPixelARGB32(x, y, 0x00000000); // Draw black or 0xFFFFFFFF for white
    }
  }
}

void displayImage(int screen_width, int screen_height, int image_width,
  int image_height, const unsigned long *image_data) {
  // Initialize the framebuffer
  framebf_init(screen_width, screen_height, image_width, image_height);

  // Clear the screen or framebuffer here
  clearFramebuffer(screen_width, screen_height); // You might need to implement this function based on your system.

  // Ensure offsets are within bounds
  if (x_offset < 0)
    x_offset = 0;
  if (y_offset < 0)
    y_offset = 0;
  if (x_offset > image_width - screen_width)
    x_offset = image_width - screen_width;
  if (y_offset > image_height - screen_height)
    y_offset = image_height - screen_height;

  // Render the image within the valid area
  for (int y = 0; y < screen_height; ++y) {
    int imgY = y + y_offset;
    if (imgY >= image_height){
      break; // Ensure we don't read past the image buffer
    }
    for (int x = 0; x < screen_width; ++x) {
      int imgX = x + x_offset;
      if (imgX >= image_width)
        break; // Ensure we don't read past the image buffer

      unsigned long pixel = image_data[imgY * image_width + imgX];
      drawPixelARGB32(x, y, pixel); // Draw each pixel
    }
  }
}

void scrollImage(char key, int screen_width, int screen_height, int image_width, int image_height, const unsigned long *image_data) {
  if (key == 'w')
    y_offset -= scroll_step; // scroll up
  if (key == 's')
    y_offset += scroll_step; // scroll down
  if (key == 'a')
    x_offset -= scroll_step; // scroll left
  if (key == 'd')
    x_offset += scroll_step; // scroll right

  // redraw image
  displayImage(screen_width, screen_height, image_width, image_height, image_data);
}
