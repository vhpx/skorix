// this file is img.h
#ifndef IMAGE_H
#define IMAGE_H

#define IMAGE_WIDTH 1920
#define IMAGE_HEIGHT 1371

void clear_frame_buffer(int width, int height);

// Function prototype for displaying an image
void display_image(int image_width, int image_height,
                   const unsigned long *image_data);
void scroll_image(char key, int image_width, int image_height,
                  const unsigned long *image_data);

#endif
