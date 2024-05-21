// game.c
#include "game.h"
#include "../../../img/img.h"
#include "../../headers/constants.h"
#include "../../headers/framebf.h"
#include "../../headers/gengine.h"
#include "../../headers/uart0.h"
#include "../../headers/unrob.h"
#include "../engine/map-bitmap.h"

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
  int paddlewidth = 40;
  int paddleheight = 40;

  draw_rect((WIDTH - paddlewidth) / 2, (HEIGHT - MARGIN - paddleheight),
            (WIDTH - paddlewidth) / 2 + paddlewidth, (HEIGHT - MARGIN), 0x11,
            1);

  unrob_objects[unrob_numobjs].type = OBJ_PADDLE;
  unrob_objects[unrob_numobjs].x = (WIDTH - paddlewidth) / 2;
  unrob_objects[unrob_numobjs].y = (HEIGHT - MARGIN - paddleheight);
  unrob_objects[unrob_numobjs].width = paddlewidth;
  unrob_objects[unrob_numobjs].height = paddleheight;
  unrob_objects[unrob_numobjs].alive = 1;
  player = &unrob_objects[unrob_numobjs];
  unrob_numobjs++;
}

void start_unrob_game() {
  display_image(SCREEN_GAME_WIDTH, SCREEN_GAME_HEIGHT, MAP_WIDTH, MAP_HEIGHT,
                gameMap_bitmap_map1);
  spawnPlayer();
}

// Function to move the player based on keyboard input
void movePlayer(char key) {
  if (!player)
    return; // Ensure player object exists

  int prev_x = player->x;
  int prev_y = player->y;
  uart_puts("\n------------------------------\n");

  switch (key) {
  case 'w': // Move up
    uart_puts("\nMoving up to: \n");
    uart_puts("position[");
    uart_dec((player->y - MOVE_STEP) / MOVE_STEP);
    uart_puts("][");
    uart_dec(player->x / MOVE_STEP);
    uart_puts("]");

    // since the player is equal to 2 STEP_MOVE, we handle the all case for
    // collision
    // if (player->y - MOVE_STEP >= 0 && player->y != 0 &&
    //     logical_map[(player->y - MOVE_STEP) / MOVE_STEP]
    //                [player->x / MOVE_STEP] &&
    //     logical_map[(player->y - MOVE_STEP) / MOVE_STEP]
    //                [(player->x + MOVE_STEP) / MOVE_STEP])
    player->y -= MOVE_STEP;
    break;

  case 's': // Move down
    uart_puts("\nMoving down to: \n");
    uart_puts("position[");
    uart_dec((player->y + player->height + MOVE_STEP) / MOVE_STEP);
    uart_puts("][");
    uart_dec(player->x / MOVE_STEP);
    uart_puts("]");

    // if (player->y + player->height + MOVE_STEP <= SCREEN_GAME_HEIGHT &&
    //     logical_map[(player->y + player->height) / MOVE_STEP]
    //                [player->x / MOVE_STEP] &&
    //     logical_map[(player->y + player->height) / MOVE_STEP]
    //                [(player->x + MOVE_STEP) / MOVE_STEP])
    player->y += MOVE_STEP;
    break;

  case 'a': // Move left
    uart_puts("\nMoving left to: \n");
    uart_puts("position[");
    uart_dec(player->y / MOVE_STEP);
    uart_puts("][");
    uart_dec((player->x - MOVE_STEP) / MOVE_STEP);
    uart_puts("]");

    // if (player->x - MOVE_STEP >= 0 && player->x != 0 &&
    //     logical_map[player->y / MOVE_STEP]
    //                [(player->x - MOVE_STEP) / MOVE_STEP] &&
    //     logical_map[(player->y + MOVE_STEP) / MOVE_STEP]
    //                [(player->x - MOVE_STEP) / MOVE_STEP])
    player->x -= MOVE_STEP;
    break;

  case 'd': // Move right
    uart_puts("\nMoving right to: \n");
    uart_puts("position[");
    uart_dec((player->y) / MOVE_STEP);
    uart_puts("][");
    uart_dec((player->x + player->width) / MOVE_STEP);
    uart_puts("]");

    if (player->x + player->width <= SCREEN_GAME_WIDTH)
      player->x += MOVE_STEP;
    break;

  default:
    return; // If the key is not one of 'w', 's', 'a', or 'd', do nothing
  }

  uart_puts("\nPosition x: ");
  uart_dec(player->x);
  uart_puts("\nPosition y: ");
  uart_dec(player->y);
  uart_puts("\n");

  // Clear the entire framebuffer and redraw the map
  display_image(SCREEN_GAME_WIDTH, SCREEN_GAME_HEIGHT, MAP_WIDTH, MAP_HEIGHT,
                gameMap_bitmap_map1);

  // Draw the new position of the player
  draw_rect(player->x, player->y, player->x + player->width,
            player->y + player->height, 0x11, 1);
}
