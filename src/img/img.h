// this file is img.h
#ifndef IMAGE_H
#define IMAGE_H

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define IMAGE_WIDTH 1920
#define IMAGE_HEIGHT 1371

void clearFramebuffer(int width, int height);

// Function prototype for displaying an image
void displayImage(int screen_width, int screen_height, int image_width,
                  int image_height, const unsigned long *image_data);
void scrollImage(char key, int screen_width, int screen_height, int image_width,
                 int image_height, const unsigned long *image_data);

#endif
