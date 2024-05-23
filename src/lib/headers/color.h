#include "constants.h"

#ifndef __COLOR_H__
#define __COLOR_H__

typedef struct {
  char BLACK[10];
  char RED[10];
  char GREEN[10];
  char YELLOW[10];
  char BLUE[10];
  char MAGENTA[10];
  char CYAN[10];
  char WHITE[10];
} TextColor;

typedef struct {
  char BLACK[10];
  char RED[10];
  char GREEN[10];
  char YELLOW[10];
  char BLUE[10];
  char MAGENTA[10];
  char CYAN[10];
  char WHITE[10];
} BackgroundColor;

typedef struct {
  TextColor TEXT;
  BackgroundColor BACKGROUND;
  char RESET[10];
} Color;

extern Color COLOR;

#endif
