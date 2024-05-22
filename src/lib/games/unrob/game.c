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
const int PLAYER_SPAWN_Y = (SCREEN_HEIGHT - MARGIN - PLAYER_HEIGHT);

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

// Function to initialize background buffer and sprite buffer
void initialize_buffers() {
  // Copy the background to the cache buffer
  copy_rect(PLAYER_SPAWN_X, PLAYER_SPAWN_Y, 0, 0, SCREEN_WIDTH, PLAYER_WIDTH,
            PLAYER_HEIGHT, game_map_1_bitmap, background_cache_buffer);

  uart_puts("\n\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" [INITIALIZED UNROB GAME]");

  // Display the map
  draw_rect_from_bitmap(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, game_map_1_bitmap);

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

// Function to move the player based on keyboard input
void move_player(char key) {
  if (!player)
    return; // Ensure player object exists

  int offsetX, offsetY;

  switch (key) {
  case 'w':
    offsetY = -STEP_SIZE;
    offsetX = 0;
    break;
  case 's':
    offsetY = STEP_SIZE;
    offsetX = 0;
    break;
  case 'a':
    offsetX = -STEP_SIZE;
    offsetY = 0;
    break;
  case 'd':
    offsetX = STEP_SIZE;
    offsetY = 0;
    break;
  default:
    uart_puts("\nInvalid key.");
    return; // Invalid key
  }

  int next_x = player->x + offsetX;
  int next_y = player->y + offsetY;

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

  // TODO: Implement boundary and collision check

  // 1. Erase the old player position from the framebuffer
  draw_rect_from_bitmap(player->x, player->y, PLAYER_WIDTH, PLAYER_HEIGHT,
                        background_cache_buffer);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" [ERASED PLAYER]");

  // 2. Update the necessary portion of the background cache
  copy_rect(next_x, next_y, 0, 0, PLAYER_WIDTH, PLAYER_WIDTH, PLAYER_HEIGHT,
            game_map_1_bitmap, background_cache_buffer);

  // 3. Redraw the player at the new position
  draw_rect_from_bitmap(next_x, next_y, PLAYER_WIDTH, PLAYER_HEIGHT,
                        player_sprite_buffer);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" [REDRAWN PLAYER]");

  // 4. Update the player object's position
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
