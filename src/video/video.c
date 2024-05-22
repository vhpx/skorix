// this file is video.c
#include "video.h"
#include "../img/img.h"
#include "../lib/headers/constants.h"
#include "../lib/headers/framebf.h"
#include "../lib/headers/utils.h"
#include "video_src.h"

void display_video(int video_width, int video_height) {
  // Loop through each frame stored in the array
  for (int i = 0; i < video_bitmap_allArray_LEN; i++) {
    draw_image(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT, video_bitmap_allArray[i]);
    wait_msec(100000);
  }
}
