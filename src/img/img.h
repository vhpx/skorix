// this file is img.h
#ifndef IMAGE_H
#define IMAGE_H

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define IMAGE_WIDTH 1920
#define IMAGE_HEIGHT 1371

void clear_frame_buffer(int width, int height);

// Function prototype for displaying an image
void display_image(int screen_width, int screen_height, int image_width,
                   int image_height, const unsigned long *image_data);
void scroll_image(char key, int screen_width, int screen_height,
                  int image_width, int image_height,
                  const unsigned long *image_data);

#endif
