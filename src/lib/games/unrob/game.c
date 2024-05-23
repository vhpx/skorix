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
#include "../engine/item.h"
#include "../engine/map-bitmap.h"

struct Object {
  unsigned int type;
  Position position;
  unsigned int width;
  unsigned int height;
  unsigned int alive;
};

enum { OBJ_NONE = 0, OBJ_PLAYER = 1 };

unsigned int unrob_numobjs = 0;
struct Object unrob_objects[MAX_GENGINE_ENTITIES];
struct Object *player;
unsigned long pre_player_movement_cache[2000];

const Position PLAYER_SPAWN = {.x = (SCREEN_WIDTH - PLAYER_WIDTH) / 2,
                               .y = (SCREEN_HEIGHT - MARGIN - PLAYER_HEIGHT) -
                                    20};

unsigned long background_cache_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];
unsigned long player_sprite_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];

Position map_wall_boundaries[] = {
    {.x = 105, .y = 145}, {.x = 105, .y = 305}, {.x = 155, .y = 305},
    {.x = 155, .y = 425}, {.x = 95, .y = 425},  {.x = 95, .y = 635},
    {.x = 155, .y = 635}, {.x = 155, .y = 745}, {.x = 105, .y = 745},
    {.x = 105, .y = 865}, {.x = 215, .y = 865}, {.x = 215, .y = 795},
    {.x = 285, .y = 795}, {.x = 285, .y = 865}, {.x = 425, .y = 865},
    {.x = 425, .y = 745}, {.x = 455, .y = 745}, {.x = 455, .y = 805},
    {.x = 465, .y = 805}, {.x = 465, .y = 805}, {.x = 465, .y = 995},
    {.x = 535, .y = 995}, {.x = 535, .y = 935}, {.x = 545, .y = 935},
    {.x = 545, .y = 805}, {.x = 555, .y = 805}, {.x = 555, .y = 745},
    {.x = 585, .y = 745}, {.x = 585, .y = 865}, {.x = 905, .y = 865},
    {.x = 905, .y = 745}, {.x = 855, .y = 745}, {.x = 855, .y = 635},
    {.x = 915, .y = 635}, {.x = 915, .y = 425}, {.x = 855, .y = 425},
    {.x = 855, .y = 305}, {.x = 905, .y = 305}, {.x = 905, .y = 145},
    {.x = 805, .y = 145}, {.x = 805, .y = 205}, {.x = 755, .y = 205},
    {.x = 755, .y = 145}, {.x = 685, .y = 145}, {.x = 685, .y = 205},
    {.x = 565, .y = 205}, {.x = 565, .y = 225}, {.x = 455, .y = 225},
    {.x = 455, .y = 215}, {.x = 375, .y = 215}, {.x = 375, .y = 145},
    {.x = 315, .y = 145}, {.x = 315, .y = 215}, {.x = 205, .y = 215},
    {.x = 205, .y = 145}, {.x = 105, .y = 145},
};

Position map_fountain_boundaries[] = {
    {.x = 445, .y = 480}, {.x = 445, .y = 550}, {.x = 470, .y = 575},
    {.x = 530, .y = 575}, {.x = 555, .y = 550}, {.x = 555, .y = 480},
    {.x = 530, .y = 455}, {.x = 470, .y = 455},
};

Boundary map_boundaries[] = {
    {.positions = map_wall_boundaries,
     .num_positions = sizeof(map_wall_boundaries) / sizeof(Position)},
    {.positions = map_fountain_boundaries,
     .num_positions = sizeof(map_fountain_boundaries) / sizeof(Position)},
};

void initialize_game() {
  unrob_objects[unrob_numobjs].type = OBJ_PLAYER;
  unrob_objects[unrob_numobjs].position.x = PLAYER_SPAWN.x;
  unrob_objects[unrob_numobjs].position.y = PLAYER_SPAWN.y;
  unrob_objects[unrob_numobjs].width = PLAYER_WIDTH;
  unrob_objects[unrob_numobjs].height = PLAYER_HEIGHT;
  unrob_objects[unrob_numobjs].alive = 1;
  player = &unrob_objects[unrob_numobjs];
  unrob_numobjs++;

  for (int i = 0; i < item_m1_allArray_LEN; i++) { // item map 1
    long long prev_pixels = get_rendered_pixels();

    draw_transparent_image(
        SCREEN_WIDTH / 2 - (7 * ITEM_SIZE) / 2 + (i * ITEM_SIZE),
        SCREEN_HEIGHT / 2 - 200, ITEM_SIZE, ITEM_SIZE, item_m1_allArray[i]);
    uart_puts("\nProcessed pixels: ");
    print_rendered_pixels();
    uart_puts(" | ");
    print_pixel_diff(prev_pixels, "[DRAWN ITEM]");
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
  long long prev_pixels = get_rendered_pixels();

  uart_puts("\n\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[RESET RENDERED PIXELS]");

  // Display the map
  draw_rect_from_bitmap(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, game_map_1_bitmap);

  // Copy the initial portion of the background to the cache buffer
  copy_rect(PLAYER_SPAWN.x, PLAYER_SPAWN.y, 0, 0, SCREEN_WIDTH, PLAYER_WIDTH,
            PLAYER_HEIGHT, game_map_1_bitmap, background_cache_buffer);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[DRAWN MAP]");

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
}

void countdown(void) {
  if (game_time) {
    game_time--;
    draw_time();
  } else {
    // Game over?
  }
}

void draw_player() {
  if (!player)
    return; // Ensure player object exists

  long long prev_pixels = get_rendered_pixels();

  // Copy the background to the player's position
  copy_rect(player->position.x, player->position.y, 0, 0, SCREEN_WIDTH,
            PLAYER_WIDTH, PLAYER_HEIGHT, game_map_1_bitmap,
            background_cache_buffer);

  // Draw the player sprite
  draw_rect_from_bitmap(player->position.x, player->position.y, PLAYER_WIDTH,
                        PLAYER_HEIGHT, player_sprite_buffer);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[DRAWN PLAYER]");
}

void draw_time() {
  game_time_str[0] = '0' + game_time / 60;
  game_time_str[2] = '0' + (game_time % 60) / 10;
  game_time_str[3] = '0' + (game_time % 60) % 10;

  long long prev_pixels = get_rendered_pixels();

  draw_rect_ARGB_32(
      SCREEN_WIDTH - strlen(game_time_str) * FONT_WIDTH * GAME_TIME_ZOOM - 1, 0,
      SCREEN_WIDTH, FONT_HEIGHT * GAME_TIME_ZOOM, 0x00000000, 1);
  draw_string(SCREEN_WIDTH -
                  strlen(game_time_str) * FONT_WIDTH * GAME_TIME_ZOOM,
              0, game_time_str, 0x00FFFFFF, GAME_TIME_ZOOM);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[DRAWN TIME]");
}

void move_player(char key) {
  if (!player)
    return; // Ensure player object exists

  move_in_boundaries(map_boundaries, sizeof(map_boundaries) / sizeof(Boundary),
                     key, &player->position, game_map_1_bitmap,
                     background_cache_buffer, player_sprite_buffer);
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
  draw_transparent_image(35, 35, ITEM_SIZE, ITEM_SIZE,
                         item_m1_allArray[selected_item]);
  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[DRAWN INVENTORY]");

  // dispay on top of the player
  //  draw_rect(player->position.x-10, player->position.y-50, 110,110,
  //  0x00ffffff, 1); draw_transparent_image(player->position.x,
  //  player->position.y - 50, ITEM_SIZE, ITEM_SIZE,
  //  item_m1_allArray[selected_item]);
}

static int collision_debugger = 0;

void toggle_collision_debugger() {
  collision_debugger = !collision_debugger;

  if (collision_debugger) {
    render_boundary(map_wall_boundaries,
                    sizeof(map_wall_boundaries) / sizeof(Position));
    render_boundary(map_fountain_boundaries,
                    sizeof(map_fountain_boundaries) / sizeof(Position));
  } else {
    draw_rect_from_bitmap(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, game_map_1_bitmap);
    draw_player();
    draw_time();
    draw_inventory(selected_item);

    for (int i = 0; i < item_m1_allArray_LEN; i++) { // item map 1
      long long prev_pixels = get_rendered_pixels();

      draw_transparent_image(
          SCREEN_WIDTH / 2 - (7 * ITEM_SIZE) / 2 + (i * ITEM_SIZE),
          SCREEN_HEIGHT / 2 - 200, ITEM_SIZE, ITEM_SIZE, item_m1_allArray[i]);
      uart_puts("\nProcessed pixels: ");
      print_rendered_pixels();
      uart_puts(" | ");
      print_pixel_diff(prev_pixels, "[DRAWN ITEM]");
    }
  }
}

int get_collision_debugger_status() { return collision_debugger; }