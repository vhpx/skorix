//this file is kernel.c
#include "../../lib/headers/cli.h"
#include "../../lib/headers/framebf.h"
#include "../../img/img.h"
#include "../../img/img_src.h"
#include "../../video/video.h"
// #include "../../video/video_src.h"

int main() { 
    // displayImage(SCREEN_WIDTH, SCREEN_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT, epd_bitmap_image);
    displayVideo(SCREEN_WIDTH, SCREEN_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT);
    return run_cli(); 
}
