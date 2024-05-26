// this file is img.c
#include "img.h"
#include "../lib/headers/constants.h"
#include "../lib/headers/framebf.h"
#include "img_src.h"

// starting offsets
int x_offset = 0;
int y_offset = 0;
const int scroll_step = 20; // step size

void display_image(int image_width, int image_height,
                   const unsigned long *image_data) {
  // Clear the screen or framebuffer here
  // clear_frame_buffer(SCREEN_WIDTH, SCREEN_HEIGHT); // You might need to
  // implement this function based on your system.

  // Ensure offsets are within bounds
  if (x_offset < 0)
    x_offset = 0;
  if (y_offset < 0)
    y_offset = 0;
  if (x_offset > image_width - SCREEN_WIDTH)
    x_offset = image_width - SCREEN_WIDTH;
  if (y_offset > image_height - SCREEN_HEIGHT)
    y_offset = image_height - SCREEN_HEIGHT;

  // Render the image within the valid area
  for (int y = 0; y < SCREEN_HEIGHT; ++y) {
    int imgY = y + y_offset;
    if (imgY >= image_height) {
      break; // Ensure we don't read past the image buffer
    }
    for (int x = 0; x < SCREEN_WIDTH; ++x) {
      int imgX = x + x_offset;
      if (imgX >= image_width)
        break; // Ensure we don't read past the image buffer

      unsigned long pixel = image_data[imgY * image_width + imgX];
      draw_pixel(x, y, pixel); // Draw each pixel
    }
  }
}

void scroll_image(char key, int image_width, int image_height,
                  const unsigned long *image_data) {
  if (key == 'w')
    y_offset -= scroll_step; // scroll up
  if (key == 's')
    y_offset += scroll_step; // scroll down
  if (key == 'a')
    x_offset -= scroll_step; // scroll left
  if (key == 'd')
    x_offset += scroll_step; // scroll right

  // redraw image
  display_image(image_width, image_height, image_data);
}
