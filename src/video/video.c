//this file is video.c
#include "video.h"
#include "../img/img.h"
#include "video_src.h"
#include "../lib/headers/utils.h"

void displayVideo(int screen_width, int screen_height, int video_width, int video_height) {
    // Loop through each frame stored in the array
    for (int i = 0; i < video_bitmap_allArray_LEN; i++) {
        displayImage(SCREEN_WIDTH_VIDEO, SCREEN_HEIGHT_VIDEO, VIDEO_WIDTH, VIDEO_HEIGHT, video_bitmap_allArray[i]);
        wait_msec(100000);
    }
}