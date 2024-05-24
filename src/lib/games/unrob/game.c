// game.c
#include "game.h"
#include "../../headers/color.h"
#include "../../headers/constants.h"
#include "../../headers/font.h"
#include "../../headers/framebf.h"
#include "../../headers/gengine.h"
#include "../../headers/interrupt.h"
#include "../../headers/print.h"
#include "../../headers/string.h"
#include "../../headers/timer.h"
#include "../../headers/uart0.h"
#include "../../headers/utils.h"
#include "../engine/item.h"
#include "../engine/map-bitmap.h"
#include "../engine/player.h"
#include "maps.h"

const GameMap *map = &map1;

unsigned int unrob_numobjs = 0;
struct Object unrob_objects[MAX_GENGINE_ENTITIES];
struct Object *player;
unsigned long pre_player_movement_cache[2000];

unsigned long background_cache_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];
unsigned long player_sprite_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];

void initialize_game() {
  //   TODO: Actually fix this
  //   Probable cause: Unfinished unrob_objects initialization
  unrob_objects[unrob_numobjs].type = OBJ_PLAYER;
  wait_msec(200);
  unrob_objects[unrob_numobjs].position.x = map1.spawn_point.x;
  wait_msec(200);
  unrob_objects[unrob_numobjs].position.y = map1.spawn_point.y;
  wait_msec(200);
  unrob_objects[unrob_numobjs].width = PLAYER_WIDTH;
  wait_msec(200);
  unrob_objects[unrob_numobjs].height = PLAYER_HEIGHT;
  wait_msec(200);
  unrob_objects[unrob_numobjs].alive = 1;
  wait_msec(200);
  player = &unrob_objects[unrob_numobjs];
  unrob_numobjs++;
}

int selected_item = 0;
unsigned int game_time = 0;
unsigned int game_score = 0;
char *game_time_str = "Time: 0:00";
char *game_score_str = "Score: 000";

void initialize_buffers() {
  long long prev_pixels = get_rendered_pixels();

  uart_puts("\n\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[RESET RENDERED PIXELS]");

  // Display the map
  draw_rect_from_bitmap_alpha(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                              game_map_1_bitmap, 50);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[DRAWN INITIAL MAP] ALPHA: 50");

  draw_items();

  wait_msec(2000);

  for (int i = 50; i <= 100; i += 5) {
    prev_pixels = get_rendered_pixels();
    char msg[MAX_STR_LENGTH];
    char alpha[4];

    clrstr(msg);
    clrstr(alpha);

    append_str(msg, "[DRAWN MAP] ALPHA: ");
    int2str(i, alpha);
    append_str(msg, alpha);

    draw_rect_from_bitmap_alpha(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                                game_map_1_bitmap, i);
    uart_puts("\nProcessed pixels: ");
    print_rendered_pixels();
    uart_puts(" | ");
    print_pixel_diff(prev_pixels, msg);
    wait_msec(300);
  }

  // Copy the initial portion of the background to the cache buffer
  copy_rect(map1.spawn_point.x, map1.spawn_point.y, 0, 0, SCREEN_WIDTH,
            PLAYER_WIDTH, PLAYER_HEIGHT, game_map_1_bitmap,
            background_cache_buffer);

  prev_pixels = get_rendered_pixels();
}

void start_unrob_game() {
  reset_rendered_pixels();
  initialize_buffers();
  initialize_game();

  draw_player();
  draw_inventory(selected_item);

  game_time = 61;
  draw_time();
  sys_timer3_init();
  sys_timer3_irq_enable();

  draw_score();
}

void countdown(void) {
  if (game_time) {
    game_time--;
    draw_time();
  } else {
    // Game over?
  }
}

enum { UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3 };
static int player_direction = UP;

const unsigned long *get_player_sprite() {
  switch (player_direction) {
  case UP:
    return player_up;
  case DOWN:
    return player_down;
  case LEFT:
    return player_left;
  case RIGHT:
    return player_right;
  default:
    return player_up;
  }
}

void draw_items() {
  for (int i = 0; i < item_m1_allArray_LEN; i++) { // item map 1
    long long prev_pixels = get_rendered_pixels();

    draw_transparent_image(SCREEN_WIDTH / 2 - (7 * GENGINE_ITEM_SIZE) / 2 +
                               (i * GENGINE_ITEM_SIZE),
                           SCREEN_HEIGHT / 2 - 200, GENGINE_ITEM_SIZE,
                           GENGINE_ITEM_SIZE, item_m1_allArray[i]);
    uart_puts("\nProcessed pixels: ");
    print_rendered_pixels();
    uart_puts(" | ");
    print_pixel_diff(prev_pixels, "[DRAWN ITEM]");
  }

  // for (int i = 0; i < item_m2_allArray_LEN; i++) { // map 2
  //   draw_transparent_image(WIDTH / 2 - (7 * GENGINE_ITEM_SIZE) / 2 + (i *
  //   GENGINE_ITEM_SIZE),
  //                          HEIGHT / 2 - 150, GENGINE_ITEM_SIZE,
  //                          GENGINE_ITEM_SIZE, item_m2_allArray[i]);
  // }

  // for (int i = 0; i < item_m3_allArray_LEN; i++) { // map 3
  //   draw_transparent_image(WIDTH / 2 - (8 * GENGINE_ITEM_SIZE) / 2 + (i *
  //   GENGINE_ITEM_SIZE),
  //                          HEIGHT / 2 - 100, GENGINE_ITEM_SIZE,
  //                          GENGINE_ITEM_SIZE, item_m3_allArray[i]);
  // }
}

void draw_player() {
  if (!player)
    return; // Ensure player object exists

  long long prev_pixels = get_rendered_pixels();

  // Copy the background to the player's position
  copy_rect(player->position.x, player->position.y, 0, 0, SCREEN_WIDTH,
            PLAYER_WIDTH, PLAYER_HEIGHT, game_map_1_bitmap,
            background_cache_buffer);

  copy_rect(0, 0, 0, 0, PLAYER_WIDTH, PLAYER_WIDTH, PLAYER_HEIGHT,
            get_player_sprite(), player_sprite_buffer);

  // Draw the player sprite
  // draw_rect_from_bitmap(player->position.x, player->position.y, PLAYER_WIDTH,
  //                       PLAYER_HEIGHT, player_sprite_buffer);
  draw_transparent_image(player->position.x, player->position.y, PLAYER_WIDTH,
                         PLAYER_HEIGHT, player_sprite_buffer);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[DRAWN PLAYER]");
}

void draw_time() {
  game_time_str[6] = '0' + game_time / 60;
  game_time_str[8] = '0' + (game_time % 60) / 10;
  game_time_str[9] = '0' + (game_time % 60) % 10;

  long long prev_pixels = get_rendered_pixels();

  draw_rect_ARGB_32(
      SCREEN_WIDTH - strlen(game_time_str) * FONT_WIDTH * GENGINE_TIME_ZOOM - 1,
      0, SCREEN_WIDTH, FONT_HEIGHT * GENGINE_TIME_ZOOM, 0x00000000, 1);
  draw_string(SCREEN_WIDTH -
                  strlen(game_time_str) * FONT_WIDTH * GENGINE_TIME_ZOOM,
              0, game_time_str, 0x00FFFFFF, GENGINE_TIME_ZOOM);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[DRAWN COUNTDOWN TIMER]");
}

void draw_score() {
  game_score_str[7] = '0' + game_score / 100;
  game_score_str[8] = '0' + (game_score / 10) % 10;
  game_score_str[9] = '0' + game_score % 100;

  long long prev_pixels = get_rendered_pixels();

  draw_rect_ARGB_32(
      SCREEN_WIDTH - strlen(game_score_str) * FONT_WIDTH * GENGINE_TIME_ZOOM -
          1,
      FONT_HEIGHT * GENGINE_TIME_ZOOM, SCREEN_WIDTH,
      (FONT_HEIGHT * GENGINE_TIME_ZOOM) * 2, 0x00000000, 1);
  draw_string(SCREEN_WIDTH -
                  strlen(game_score_str) * FONT_WIDTH * GENGINE_TIME_ZOOM,
              FONT_HEIGHT * GENGINE_TIME_ZOOM, game_score_str, 0x00FFFFFF,
              GENGINE_TIME_ZOOM);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[DRAWN SCORE]");
}

void move_player(char key) {
  if (!player)
    return; // Ensure player object exists

  int force_redraw = false;

  switch (key) {
  case 'w':
    if (player_direction != UP) {
      player_direction = UP;
      force_redraw = true;
    }
    break;
  case 's':
    if (player_direction != DOWN) {
      player_direction = DOWN;
      force_redraw = true;
    }
    break;
  case 'a':
    if (player_direction != LEFT) {
      player_direction = LEFT;
      force_redraw = true;
    }
    break;
  case 'd':
    if (player_direction != RIGHT) {
      player_direction = RIGHT;
      force_redraw = true;
    }
    break;

  default:
    return; // Do nothing if another key is pressed
  }

  if (force_redraw)
    copy_rect(0, 0, 0, 0, PLAYER_WIDTH, PLAYER_WIDTH, PLAYER_HEIGHT,
              get_player_sprite(), player_sprite_buffer);

  move_in_boundaries(map->boundaries, map->num_boundaries, key,
                     &player->position, game_map_1_bitmap,
                     background_cache_buffer, player_sprite_buffer,
                     force_redraw);
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
    return; // Do nothing if another key is pressed
  }

  uart_puts("\n\nReceived key: ");
  uart_puts(COLOR.TEXT.BLUE);
  char2upper(&key);
  uart_sendc(key);
  uart_puts(COLOR.RESET);

  // Display the selected item in the inventory
  draw_inventory(selected_item);
}

void draw_inventory(int selected_item) {
  long long prev_pixels = get_rendered_pixels();

  // display top right corner
  draw_rect(0, 0, 110, 110, 0x00ffffff, 1);
  draw_transparent_image(35, 35, GENGINE_ITEM_SIZE, GENGINE_ITEM_SIZE,
                         item_m1_allArray[selected_item]);
  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[DRAWN INVENTORY]");

  // dispay on top of the player
  //  draw_rect(player->position.x-10, player->position.y-50, 110,110,
  //  0x00ffffff, 1); draw_transparent_image(player->position.x,
  //  player->position.y - 50, GENGINE_ITEM_SIZE, GENGINE_ITEM_SIZE,
  //  item_m1_allArray[selected_item]);
}

static int collision_debugger = 0;

void toggle_collision_debugger() {
  collision_debugger = !collision_debugger;

  if (collision_debugger) {
    render_boundaries(map->boundaries, map->num_boundaries);
  } else {
    long long prev_pixels = get_rendered_pixels();

    draw_rect_from_bitmap(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, game_map_1_bitmap);
    uart_puts("\nProcessed pixels: ");
    print_rendered_pixels();
    uart_puts(" | ");
    print_pixel_diff(prev_pixels, "[DRAWN MAP]");

    draw_player();
    draw_time();
    draw_inventory(selected_item);

    for (int i = 0; i < item_m1_allArray_LEN; i++) { // item map 1
      prev_pixels = get_rendered_pixels();

      draw_transparent_image(SCREEN_WIDTH / 2 - (7 * GENGINE_ITEM_SIZE) / 2 +
                                 (i * GENGINE_ITEM_SIZE),
                             SCREEN_HEIGHT / 2 - 200, GENGINE_ITEM_SIZE,
                             GENGINE_ITEM_SIZE, item_m1_allArray[i]);
      uart_puts("\nProcessed pixels: ");
      print_rendered_pixels();
      uart_puts(" | ");
      print_pixel_diff(prev_pixels, "[DRAWN ITEM]");
    }
  }
}

int get_collision_debugger_status() { return collision_debugger; }