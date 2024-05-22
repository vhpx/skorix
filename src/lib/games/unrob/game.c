// game.c
#include "game.h"
#include "../../headers/constants.h"
#include "../../headers/font.h"
#include "../../headers/framebf.h"
#include "../../headers/gengine.h"
#include "../../headers/interrupt.h"
#include "../../headers/print.h"
#include "../../headers/string.h"
#include "../../headers/timer.h"
#include "../../headers/uart0.h"
#include "../engine/item.h"
#include "../engine/map-bitmap.h"

#define PLAYER_WIDTH 40
#define PLAYER_HEIGHT 40

struct Object {
  unsigned int type;
  unsigned int x;
  unsigned int y;
  unsigned int width;
  unsigned int height;
  unsigned int alive;
};

enum { OBJ_NONE = 0, OBJ_BRICK = 1, OBJ_PADDLE = 2, OBJ_BALL = 3 };

unsigned int unrob_numobjs = 0;
struct Object unrob_objects[MAX_GENGINE_ENTITIES];
struct Object *player;
unsigned long pre_player_movement_cache[2000];

const int PLAYER_SPAWN_X = (SCREEN_WIDTH - PLAYER_WIDTH) / 2;
const int PLAYER_SPAWN_Y = (SCREEN_HEIGHT - MARGIN - PLAYER_HEIGHT) - 20;

unsigned long background_cache_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];
unsigned long player_sprite_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];

void initialize_game() {
  unrob_objects[unrob_numobjs].type = OBJ_PADDLE;
  unrob_objects[unrob_numobjs].x = PLAYER_SPAWN_X;
  unrob_objects[unrob_numobjs].y = PLAYER_SPAWN_Y;
  unrob_objects[unrob_numobjs].width = PLAYER_WIDTH;
  unrob_objects[unrob_numobjs].height = PLAYER_HEIGHT;
  unrob_objects[unrob_numobjs].alive = 1;
  player = &unrob_objects[unrob_numobjs];
  unrob_numobjs++;

  for (int i = 0; i < item_m1_allArray_LEN; i++) { // item map 1
    draw_transparent_image(
        SCREEN_WIDTH / 2 - (7 * ITEM_SIZE) / 2 + (i * ITEM_SIZE),
        SCREEN_HEIGHT / 2 - 200, ITEM_SIZE, ITEM_SIZE, item_m1_allArray[i]);
    uart_puts("\nProcessed pixels: ");
    print_rendered_pixels();
    uart_puts(" [DRAWN ITEM]");
  }
  // for(int i = 0; i < item_m2_allArray_LEN; i++) {//map 2
  //   draw_transparent_image(WIDTH/2 - (7*ITEM_SIZE)/2 + (i*ITEM_SIZE),
  //   HEIGHT/2 -150, ITEM_SIZE,
  //                          ITEM_SIZE, item_m2_allArray[i]);
  // }
  //   for(int i = 0; i < item_m3_allArray_LEN; i++) {//map 3
  //   draw_transparent_image(WIDTH/2 - (8*ITEM_SIZE)/2 + (i*ITEM_SIZE),
  //   HEIGHT/2 -100, ITEM_SIZE,
  //                          ITEM_SIZE, item_m3_allArray[i]);
  // }
}

int selected_item = 0;
unsigned int game_time = 0;
char *game_time_str = "0:00";

void initialize_buffers() {
  uart_puts("\n\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" [INITIALIZED UNROB GAME]");

  // Display the map
  draw_rect_from_bitmap(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, game_map_1_bitmap);

  // Copy the initial portion of the background to the cache buffer
  copy_rect(PLAYER_SPAWN_X, PLAYER_SPAWN_Y, 0, 0, SCREEN_WIDTH, PLAYER_WIDTH,
            PLAYER_HEIGHT, game_map_1_bitmap, background_cache_buffer);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" [DRAWN MAP]");

  // Display the player sprite
  draw_rect_from_bitmap(PLAYER_SPAWN_X, PLAYER_SPAWN_Y, PLAYER_WIDTH,
                        PLAYER_HEIGHT, player_sprite_buffer);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" [DRAWN PLAYER]");
}

void start_unrob_game() {
  initialize_buffers();
  initialize_game();
  display_inventory(selected_item);

  game_time = 61;
  draw_time();
  sys_timer3_init();
  sys_timer3_irq_enable();
}

void countdown(void) {
  if (game_time) {
    game_time--;
    draw_time();
  } else {
    // Game over?
  }
}

void draw_time(void) {
  game_time_str[0] = '0' + game_time / 60;
  game_time_str[2] = '0' + (game_time % 60) / 10;
  game_time_str[3] = '0' + (game_time % 60) % 10;
  draw_rect_ARGB_32(
      SCREEN_WIDTH - strlen(game_time_str) * FONT_WIDTH * GAME_TIME_ZOOM - 1, 0,
      SCREEN_WIDTH, FONT_HEIGHT * GAME_TIME_ZOOM, 0x00000000, 1);
  draw_string(SCREEN_WIDTH -
                  strlen(game_time_str) * FONT_WIDTH * GAME_TIME_ZOOM,
              0, game_time_str, 0x00FFFFFF, GAME_TIME_ZOOM);
  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" [DRAWN TIME]");
}

void move_player(char key) {
  if (!player)
    return; // Ensure player object exists

  int offsetX = 0, offsetY = 0;

  switch (key) {
  case 'w':
    offsetY = -STEP_SIZE;
    break;
  case 's':
    offsetY = STEP_SIZE;
    break;
  case 'a':
    offsetX = -STEP_SIZE;
    break;
  case 'd':
    offsetX = STEP_SIZE;
    break;
  default:
    uart_puts("\nInvalid key.");
    return; // Invalid key
  }

  int next_x = player->x + offsetX;
  int next_y = player->y + offsetY;

  // Boundary check
  if (next_x < 0 || next_y < 0 || next_x + PLAYER_WIDTH > SCREEN_WIDTH ||
      next_y + PLAYER_HEIGHT > SCREEN_HEIGHT) {
    return; // Out of bounds
  }

  // Display position change
  uart_puts("\n\nReceived key: ");
  uart_sendc(key);

  uart_puts("\nPlayer position: ");
  uart_puts("(");
  uart_dec(player->x);
  uart_puts(", ");
  uart_dec(player->y);
  uart_puts(") -> (");
  uart_dec(next_x);
  uart_puts(", ");
  uart_dec(next_y);
  uart_puts(")\n");

  // Calculate rectangular regions that need to be updated
  int erase_x = player->x;
  int erase_y = player->y;
  int redraw_x = next_x;
  int redraw_y = next_y;
  int update_width = PLAYER_WIDTH;
  int update_height = PLAYER_HEIGHT;

  // Adjust regions if the player moved partially off-screen
  if (erase_x < 0) {
    erase_x = 0;
    update_width += player->x;
  }
  if (erase_y < 0) {
    erase_y = 0;
    update_height += player->y;
  }
  if (redraw_x + PLAYER_WIDTH > SCREEN_WIDTH) {
    update_width = SCREEN_WIDTH - redraw_x;
  }
  if (redraw_y + PLAYER_HEIGHT > SCREEN_HEIGHT) {
    update_height = SCREEN_HEIGHT - redraw_y;
  }

  // Update only the necessary portions of the background and sprite
  draw_rect_from_bitmap(erase_x, erase_y, update_width, update_height,
                        background_cache_buffer);
  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" [ERASED PLAYER]");

  copy_rect(redraw_x, redraw_y, 0, 0, SCREEN_WIDTH, update_width, update_height,
            game_map_1_bitmap, background_cache_buffer);
  draw_rect_from_bitmap(redraw_x, redraw_y, update_width, update_height,
                        player_sprite_buffer);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" [REDRAWN PLAYER]");

  // Update the player object's position
  player->x = next_x;
  player->y = next_y;
}

void rotate_inventory(char key) {
  switch (key) {
  case 'q': // Rotate left
    selected_item--;

    // Wrap around if the index goes below 0
    if (selected_item < 0) {
      selected_item = item_m1_allArray_LEN - 1;
    }
    break;

  case 'e': // Rotate right
    selected_item++;

    // Wrap around if the index exceeds array length
    if (selected_item >= item_m1_allArray_LEN) {
      selected_item = 0;
    }
    break;

  default:
    break; // Do nothing if another key is pressed
  }

  // Display the selected item in the inventory
  display_inventory(selected_item);
}

void display_inventory(int selected_item) {
  // display top right corner
  draw_rect(0, 0, 110, 110, 0x00ffffff, 1);
  draw_transparent_image(35, 35, ITEM_SIZE, ITEM_SIZE,
                         item_m1_allArray[selected_item]);
  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" [DRAWN INVENTORY]");

  // dispay on top of the player
  //  draw_rect(player->x-10, player->y-50, 110,110, 0x00ffffff, 1);
  //  draw_transparent_image(player->x, player->y - 50, ITEM_SIZE, ITEM_SIZE,
  //  item_m1_allArray[selected_item]);
}
