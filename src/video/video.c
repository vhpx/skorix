//this file is video.c
#include "video.h"
#include "../img/img.h"
#include "video_src.h"

void wait_msec(unsigned int n) {
    register unsigned long f, t, r;

    // Get the current counter frequency
    asm volatile("mrs %0, cntfrq_el0" : "=r" (f));

    // Calculate the number of clock cycles needed to wait
    unsigned long cyclesToWait = (f / 1000) * n;

    // Read the current counter value
    asm volatile("mrs %0, cntpct_el0" : "=r" (t));

    // Calculate expire value for counter
    unsigned long expiredTime = t + cyclesToWait;

    // Busy-wait until the counter reaches the expired time
    do {
        asm volatile("mrs %0, cntpct_el0" : "=r" (r));
    } while (r < expiredTime);
}

void displayVideo(int screen_width, int screen_height, int video_width, int video_height) {
    // Loop through each frame stored in the array
    for (int i = 0; i < video_bitmap_allArray_LEN; i++) {
        displayImage(SCREEN_WIDTH_VIDEO, SCREEN_HEIGHT_VIDEO, VIDEO_WIDTH, VIDEO_HEIGHT, video_bitmap_allArray[i]);
        wait_msec(50); //not working
    }
}