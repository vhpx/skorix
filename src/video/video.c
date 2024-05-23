// this file is video.c
#include "video.h"
#include "../img/img.h"
#include "../lib/headers/cli.h"
#include "../lib/headers/constants.h"
#include "../lib/headers/framebf.h"
#include "../lib/headers/interrupt.h"
#include "../lib/headers/utils.h"
#include "video_src.h"


int video_pause = 0;
int video_exit = 0;
int video_end = 1;
int video_restart = 0;

void display_video(int video_width, int video_height) {
  video_pause = 0;
  video_exit = 0;
  video_end = 0;
  video_restart = 0;
  interrupt_enable();

  // Loop through each frame stored in the array
  for (int i = 0; i < video_bitmap_allArray_LEN; i++) {
    draw_image(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT, video_bitmap_allArray[i]);
    wait_msec(100);

    if (video_pause) {
      i--;
    }

    if (video_exit) {
      break;
    }

    if (video_restart) {
      i = 0;
      video_pause = 0;
      video_restart = 0;
    }
  }

  interrupt_disable();
  clear_frame_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);
  video_end = 1;
}
