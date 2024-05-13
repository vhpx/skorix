#include "../../headers/constants.h"
#include "../../headers/framebf.h"
#include "../../headers/gengine.h"
#include "../../headers/unrob.h"

// The screen
#define WIDTH 1280
#define HEIGHT 720
#define MARGIN 30
#define VIRTWIDTH (WIDTH - (2 * MARGIN))
#define FONT_BPG 8

struct Object {
  unsigned int type;
  unsigned int x;
  unsigned int y;
  unsigned int width;
  unsigned int height;
  unsigned char alive;
};

enum { OBJ_NONE = 0, OBJ_BRICK = 1, OBJ_PADDLE = 2, OBJ_BALL = 3 };

unsigned int unrob_numobjs = 0;
struct Object unrob_objects[MAX_GENGINE_ENTITIES];
struct Object *player;

void spawnPlayer() {
  int paddlewidth = 80;
  int paddleheight = 20;

  drawRect((WIDTH - paddlewidth) / 2, (HEIGHT - MARGIN - paddleheight),
           (WIDTH - paddlewidth) / 2 + paddlewidth, (HEIGHT - MARGIN), 0x11, 1);

  unrob_objects[unrob_numobjs].type = OBJ_PADDLE;
  unrob_objects[unrob_numobjs].x = (WIDTH - paddlewidth) / 2;
  unrob_objects[unrob_numobjs].y = (HEIGHT - MARGIN - paddleheight);
  unrob_objects[unrob_numobjs].width = paddlewidth;
  unrob_objects[unrob_numobjs].height = paddleheight;
  unrob_objects[unrob_numobjs].alive = 1;
  player = &unrob_objects[unrob_numobjs];
  unrob_numobjs++;
}

void start_unrob_game() { spawnPlayer(); }