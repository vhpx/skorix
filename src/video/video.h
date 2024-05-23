// this file is video.h
#ifndef VIDEO_H
#define VIDEO_H

#define VIDEO_WIDTH 320
#define VIDEO_HEIGHT 180

extern int video_pause;
extern int video_exit;
extern int video_end;
extern int video_restart;

// Function prototype for displaying an image
void display_video(int image_width, int image_height);

#endif
