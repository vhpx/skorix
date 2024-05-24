// game.c
#include "game.h"
#include "../../headers/color.h"
#include "../../headers/constants.h"
#include "../../headers/font.h"
#include "../../headers/framebf.h"
#include "../../headers/gengine.h"
#include "../../headers/interrupt.h"
#include "../../headers/math.h"
#include "../../headers/print.h"
#include "../../headers/string.h"
#include "../../headers/timer.h"
#include "../../headers/uart0.h"
#include "../../headers/utils.h"
#include "../engine/item.h"
#include "../engine/map-bitmap.h"
#include "../engine/player.h"
#include "maps.h"

GameMap *map = &map1;

unsigned int unrob_numobjs = 0;
struct Object unrob_objects[MAX_GENGINE_ENTITIES];
struct Object *player;
struct Object *guard1;
struct Object *guard2;
unsigned long pre_player_movement_cache[2000];

const Position PLAYER_SPAWN = {.x = (SCREEN_WIDTH - PLAYER_WIDTH) / 2,
                               .y = (SCREEN_HEIGHT - MARGIN - PLAYER_HEIGHT) -
                                    20};
const Position GUARD1_SPAWN = {.x = 250,
                              .y = (SCREEN_HEIGHT)/2-200};       
                                                           
const Position GUARD2_SPAWN = {.x = 750,
                              .y = (SCREEN_HEIGHT)/2+200}; 

unsigned long background_cache_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];
unsigned long player_sprite_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];

unsigned long background_guard1_cache_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];
unsigned long guard1_sprite_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];
unsigned long background_guard2_cache_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];
unsigned long guard2_sprite_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];

//guard direction
//1: up
//2: right
//3: down
//4: left
static int guard1_direction = 2;
static int guard2_direction = 4;


Position map_wall_boundaries[] = {
    {.x = 105, .y = 145}, {.x = 105, .y = 305}, {.x = 155, .y = 305},
    {.x = 155, .y = 415}, {.x = 95, .y = 415},  {.x = 95, .y = 635},
    {.x = 155, .y = 635}, {.x = 155, .y = 735}, {.x = 105, .y = 735},
    {.x = 105, .y = 865}, {.x = 215, .y = 865}, {.x = 215, .y = 795},
    {.x = 275, .y = 795}, {.x = 275, .y = 865}, {.x = 435, .y = 865},
    {.x = 435, .y = 745}, {.x = 455, .y = 745}, {.x = 455, .y = 805},
    {.x = 465, .y = 805}, {.x = 465, .y = 805}, {.x = 465, .y = 995},
    {.x = 535, .y = 995}, {.x = 535, .y = 935}, {.x = 545, .y = 935},
    {.x = 545, .y = 805}, {.x = 555, .y = 805}, {.x = 555, .y = 745},
    {.x = 575, .y = 745}, {.x = 575, .y = 865}, {.x = 905, .y = 865},
    {.x = 905, .y = 745}, {.x = 855, .y = 745}, {.x = 855, .y = 635},
    {.x = 915, .y = 635}, {.x = 915, .y = 425}, {.x = 855, .y = 425},
    {.x = 855, .y = 305}, {.x = 905, .y = 305}, {.x = 905, .y = 145},
    {.x = 805, .y = 145}, {.x = 805, .y = 205}, {.x = 755, .y = 205},
    {.x = 755, .y = 145}, {.x = 675, .y = 145}, {.x = 675, .y = 205},
    {.x = 555, .y = 205}, {.x = 555, .y = 215}, {.x = 455, .y = 215},
    {.x = 455, .y = 205}, {.x = 375, .y = 205}, {.x = 375, .y = 145},
    {.x = 305, .y = 145}, {.x = 305, .y = 215}, {.x = 205, .y = 215},
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
  unrob_objects[unrob_numobjs].position.x = map1.spawn_point.x;
  unrob_objects[unrob_numobjs].position.y = map1.spawn_point.y;
  unrob_objects[unrob_numobjs].width = PLAYER_WIDTH;
  unrob_objects[unrob_numobjs].height = PLAYER_HEIGHT;
  unrob_objects[unrob_numobjs].alive = 1;
  player = &unrob_objects[unrob_numobjs];
  unrob_numobjs++;

  unrob_objects[unrob_numobjs].type = OBJ_PLAYER;
  unrob_objects[unrob_numobjs].position.x = GUARD1_SPAWN.x;
  unrob_objects[unrob_numobjs].position.y = GUARD1_SPAWN.y;
  unrob_objects[unrob_numobjs].width = PLAYER_WIDTH;
  unrob_objects[unrob_numobjs].height = PLAYER_HEIGHT;
  unrob_objects[unrob_numobjs].alive = 99;
  guard1 = &unrob_objects[unrob_numobjs];
  unrob_numobjs++;

  unrob_objects[unrob_numobjs].type = OBJ_PLAYER;
  unrob_objects[unrob_numobjs].position.x = GUARD1_SPAWN.x;
  unrob_objects[unrob_numobjs].position.y = GUARD1_SPAWN.y;
  unrob_objects[unrob_numobjs].width = PLAYER_WIDTH;
  unrob_objects[unrob_numobjs].height = PLAYER_HEIGHT;
  unrob_objects[unrob_numobjs].alive = 99;
  guard1 = &unrob_objects[unrob_numobjs];
  unrob_numobjs++;

  unrob_objects[unrob_numobjs].type = OBJ_PLAYER;
  unrob_objects[unrob_numobjs].position.x = GUARD2_SPAWN.x;
  unrob_objects[unrob_numobjs].position.y = GUARD2_SPAWN.y;
  unrob_objects[unrob_numobjs].width = PLAYER_WIDTH;
  unrob_objects[unrob_numobjs].height = PLAYER_HEIGHT;
  unrob_objects[unrob_numobjs].alive = 99;
  guard2 = &unrob_objects[unrob_numobjs];
  unrob_numobjs++;

  unrob_objects[unrob_numobjs].type = OBJ_PLAYER;
  unrob_objects[unrob_numobjs].position.x = GUARD2_SPAWN.x;
  unrob_objects[unrob_numobjs].position.y = GUARD2_SPAWN.y;
  unrob_objects[unrob_numobjs].width = PLAYER_WIDTH;
  unrob_objects[unrob_numobjs].height = PLAYER_HEIGHT;
  unrob_objects[unrob_numobjs].alive = 99;
  guard2 = &unrob_objects[unrob_numobjs];
  unrob_numobjs++;

  for (int i = 0; i < map->num_items; i++) {
    map->items[i].entity.position = map->spawn_point;
  }
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
  draw_rect_from_bitmap_alpha(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, map->bitmap,
                              50);
    // wait_msec(5000);

  for (int i = 50; i < 100; i += 5) {
    draw_rect_from_bitmap_alpha(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                                game_map_1_bitmap, i);
    wait_msec(100);
  }

  // Copy the initial portion of the background to the cache buffer
  copy_rect(PLAYER_SPAWN.x, PLAYER_SPAWN.y, 0, 0, SCREEN_WIDTH, PLAYER_WIDTH,
            PLAYER_HEIGHT, game_map_1_bitmap, background_cache_buffer);
  copy_rect(GUARD1_SPAWN.x, GUARD1_SPAWN.y, 0, 0, SCREEN_WIDTH, PLAYER_WIDTH,
          PLAYER_HEIGHT, game_map_1_bitmap, background_guard1_cache_buffer);
  copy_rect(GUARD2_SPAWN.x, GUARD2_SPAWN.y, 0, 0, SCREEN_WIDTH, PLAYER_WIDTH,
        PLAYER_HEIGHT, game_map_1_bitmap, background_guard2_cache_buffer);   

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[DRAWN INITIAL MAP] ALPHA: 50");

  move_items_to_final_position();

  wait_msec(1000);

  for (int i = 50; i <= 100; i += 5) {
    prev_pixels = get_rendered_pixels();
    char msg[MAX_STR_LENGTH];
    char alpha[4];

    clrstr(msg);
    clrstr(alpha);

    append_str(msg, "[DRAWN MAP] ALPHA: ");
    int2str(i, alpha);
    append_str(msg, alpha);

    draw_rect_from_bitmap_alpha(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, map->bitmap,
                                i);
    uart_puts("\nProcessed pixels: ");
    print_rendered_pixels();
    uart_puts(" | ");
    print_pixel_diff(prev_pixels, msg);
    wait_msec(100);
  }

  // Copy the initial portion of the background to the cache buffer
  copy_rect(map1.spawn_point.x, map1.spawn_point.y, 0, 0, SCREEN_WIDTH,
            PLAYER_WIDTH, PLAYER_HEIGHT, map->bitmap, background_cache_buffer);

  prev_pixels = get_rendered_pixels();
}

void draw_guard(struct Object *guard, unsigned long *background_cache_buffer, unsigned long *guard_sprite_buffer) {
  if (!guard)
    return; // Ensure player object exists

  long long prev_pixels = get_rendered_pixels();

  // Copy the background to the player's position
  copy_rect(guard->position.x, guard->position.y, 0, 0, SCREEN_WIDTH,
            PLAYER_WIDTH, PLAYER_HEIGHT, game_map_1_bitmap,
            background_cache_buffer);
  // Draw the player sprite
  draw_rect_from_bitmap(guard->position.x, guard->position.y, PLAYER_WIDTH,
                        PLAYER_HEIGHT, guard_sprite_buffer);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[DRAWN GUARD]");

}

//1: up
//2: right
//3: down
//4: left
void move_guard(struct Object *guard, int max_x, int max_y, int min_x, int min_y, int step_size, int *guard_direction, unsigned long *background_cache_buffer, unsigned long *guard_sprite_buffer) {
  if (!guard)
    return; // Ensure player object exists
  //upadte guard position
  if(*guard_direction == 1){
    guard->position.y -= step_size;
  }else if(*guard_direction == 2){
    guard->position.x += step_size;
  }else if(*guard_direction == 3){
    guard->position.y += step_size;
  }else if(*guard_direction == 4){
    guard->position.x -= step_size;
  }

  //update guard direction
    if(*guard_direction == 1){
      if(guard->position.y <= min_y){
      *guard_direction = 2;
      }
    }else if(*guard_direction == 2){
      if(guard->position.x >= max_x){
      *guard_direction = 3;
      }
    }else if(*guard_direction == 3){
      if(guard->position.y >= max_y){
      *guard_direction = 4;
      }
    }else if(*guard_direction == 4){
      if(guard->position.x <= min_x){
      *guard_direction = 1;
      }
    }
  // }
  
  //draw guard
    if(*guard_direction == 1){
      move_in_boundaries(map_boundaries, sizeof(map_boundaries) / sizeof(Boundary),
                        'w', &guard->position, game_map_1_bitmap,
                        background_cache_buffer, guard_sprite_buffer);
    }else if(*guard_direction == 2){
      move_in_boundaries(map_boundaries, sizeof(map_boundaries) / sizeof(Boundary),
                        'd', &guard->position, game_map_1_bitmap,
                        background_cache_buffer, guard_sprite_buffer);
    }else if(*guard_direction == 3){
      move_in_boundaries(map_boundaries, sizeof(map_boundaries) / sizeof(Boundary),
                        's', &guard->position, game_map_1_bitmap,
                        background_cache_buffer, guard_sprite_buffer);
    }else if(*guard_direction == 4){
      move_in_boundaries(map_boundaries, sizeof(map_boundaries) / sizeof(Boundary),
                        'a', &guard->position, game_map_1_bitmap,
                        background_cache_buffer, guard_sprite_buffer);
    }
}

void start_unrob_game() {
  // turn off debugger upon game start
  collision_debugger = 0;
  reset_rendered_pixels();

  initialize_game();
  initialize_buffers();

  draw_player();
  draw_guard(guard1, background_guard1_cache_buffer, guard1_sprite_buffer);
  draw_guard(guard2, background_guard2_cache_buffer, guard2_sprite_buffer);

  draw_inventory(selected_item);
  draw_placement_boxes(map->items, map->num_items, EMPTY_BOX);

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

    move_guard(guard1, 750, SCREEN_HEIGHT/2-200, 250, SCREEN_HEIGHT/2-200, GENGINE_STEP_SIZE, &guard1_direction, background_guard1_cache_buffer, guard1_sprite_buffer);
    move_guard(guard2, 750, SCREEN_HEIGHT/2+200, 250, SCREEN_HEIGHT/2+200, GENGINE_STEP_SIZE, &guard2_direction, background_guard2_cache_buffer, guard2_sprite_buffer);
  } else {
    // Game over?
  }
}

enum { UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3 };
static int player_direction = UP;

const Bitmap *get_player_sprite() {
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

void move_items_to_final_position() {
  for (int i = 0; i < map->num_items; i++) {
    float time = 0.0f;
    float speed = 0.005f; // Adjust this value to control the speed of movement

    const Item item = map->items[i];
    const Position final_position = item.final_position;
    Position current_position = item.entity.position;
    const Bitmap *bitmap = item.entity.bitmap;
    const Size size = item.entity.size;

    // Create a background cache buffer for this item
    Bitmap background_cache[size.width * size.height];

    while (time < 1.0f) {
      long long prev_pixels = get_rendered_pixels();
      char msg[MAX_STR_LENGTH];
      clrstr(msg);

      append_str(msg, "[MOVED ITEM] Name: ");
      append_str(msg, map->items[i].name);

      // Get the target position
      Position target_position = final_position;

      // Calculate the new position
      Position new_position;
      new_position.x =
          (1.0f - time) * current_position.x + time * target_position.x;
      new_position.y =
          (1.0f - time) * current_position.y + time * target_position.y;

      // If the item is very near its final position, instantly teleport to it
      if (abs(new_position.x - final_position.x) < 5.0f &&
          abs(new_position.y - final_position.y) < 5.0f) {
        new_position = final_position;
        time = 1.0f; // End the loop
      }

      // Erase the item at its previous position by applying the background
      // cache
      draw_rect_from_bitmap(current_position.x, current_position.y, size.width,
                            size.height, background_cache);

      // Update the background cache to the current background
      copy_rect_alpha(current_position.x, current_position.y, 0, 0,
                      SCREEN_WIDTH, size.width, size.height, map->bitmap,
                      background_cache, 100);

      // Copy the background to the item's new position with 50% opacity
      copy_rect_alpha(new_position.x, new_position.y, 0, 0, SCREEN_WIDTH,
                      size.width, size.height, map->bitmap, background_cache,
                      50);

      // Draw the item sprite
      draw_transparent_image(new_position.x, new_position.y, size.width,
                             size.height, item.entity.bitmap);

      // Update the item's position
      current_position = new_position;
      map->items[i].entity.position = new_position;

      // Increase the time
      time += speed;

      // Wait for a short period of time before the next update
      wait_msec(10);

      uart_puts("\nProcessed pixels: ");
      print_rendered_pixels();
      uart_puts(" | ");
      print_pixel_diff(prev_pixels, msg);
    }
  }
}

void update_placement_boxes(Position player_position, Item *items,
                            int num_items) {
  if (collision_debugger)
    return; // Do not update placement boxes if the collision debugger is on

  static int last_item_in_range_index = -1;

  for (int i = 0; i < num_items; i++) {
    float dx = (player_position.x + PLAYER_WIDTH / 2.0) -
               (items[i].final_position.x + GENGINE_ITEM_SIZE / 2.0);
    float dy = (player_position.y + PLAYER_HEIGHT / 2.0) -
               (items[i].final_position.y + GENGINE_ITEM_SIZE / 2.0);
    float distance = sqrt(dx * dx + dy * dy);

    if (distance <= GENGINE_PLACEMENT_RANGE) {
      draw_placement_boxes(
          &items[i], 1,
          IN_RANGE_BOX); // Draw the box in yellow for this item only
      last_item_in_range_index = i;
    } else if (i == last_item_in_range_index) {
      if (items[i].placement_position.x != items[i].final_position.x ||
          items[i].placement_position.y != items[i].final_position.y) {
        draw_placement_boxes(
            &items[i], 1,
            EMPTY_BOX); // Draw the box in white for this item only
      }
      last_item_in_range_index = -1; // Reset the last item in range index
    }
  }
}

void draw_placement_boxes(Item *items, int num_items, int status) {
  unsigned int color;

  switch (status) {
  case EMPTY_BOX:
    color = 0xFFFFFF; // White
    break;
  case INCORRECT_BOX:
    color = 0xFF0000; // Red
    break;
  case CORRECT_BOX:
    color = 0x00FF00; // Green
    break;
  case IN_RANGE_BOX:
    color = 0xFFFF00; // Yellow
    break;
  default:
    color = 0xFFFFFF; // Default to white
    break;
  }

  for (int i = 0; i < num_items; i++) {
    long long prev_pixels = get_rendered_pixels();
    char msg[MAX_STR_LENGTH];
    clrstr(msg);

    append_str(msg, "[DRAWN PLACE DOWN BOX] Name: ");
    append_str(msg, items[i].name);

    int x = items[i].final_position.x;
    int y = items[i].final_position.y;
    int width = GENGINE_ITEM_SIZE;
    int height = GENGINE_ITEM_SIZE; // Ensure the box is a square

    // Draw the box using lines
    draw_line(x, y + 2, x + width, y + 2, color, 4);           // Top line
    draw_line(x + 2, y, x + 2, y + height, color, 4);          // Left line
    draw_line(x + width, y, x + width, y + height, color, 4);  // Right line
    draw_line(x, y + height, x + width, y + height, color, 4); // Bottom line

    uart_puts("\nProcessed pixels: ");
    print_rendered_pixels();
    uart_puts(" | ");
    print_pixel_diff(prev_pixels, msg);
  }
}

void draw_player() {
  if (!player)
    return; // Ensure player object exists

  long long prev_pixels = get_rendered_pixels();

  // Copy the background to the player's position
  copy_rect(player->position.x, player->position.y, 0, 0, SCREEN_WIDTH,
            PLAYER_WIDTH, PLAYER_HEIGHT, map->bitmap, background_cache_buffer);

  copy_rect(0, 0, 0, 0, PLAYER_WIDTH, PLAYER_WIDTH, PLAYER_HEIGHT,
            get_player_sprite(), player_sprite_buffer);

  // Draw the player sprite
  // draw_rect_from_bitmap(player->position.x, player->position.y,
  // PLAYER_WIDTH,
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

  draw_rect(SCREEN_WIDTH -
                strlen(game_time_str) * FONT_WIDTH * GENGINE_TIME_ZOOM - 1,
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

  draw_rect(SCREEN_WIDTH -
                strlen(game_score_str) * FONT_WIDTH * GENGINE_TIME_ZOOM - 1,
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
                     &player->position, map->bitmap, background_cache_buffer,
                     player_sprite_buffer, force_redraw);
  update_placement_boxes(player->position, map->items, map->num_items);
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

void draw_items() {
  long long prev_pixels;

  for (int i = 0; i < map->num_items; i++) {
    prev_pixels = get_rendered_pixels();

    draw_transparent_image(map->items[i].final_position.x,
                           map->items[i].final_position.y, GENGINE_ITEM_SIZE,
                           GENGINE_ITEM_SIZE, map->items[i].entity.bitmap);
    uart_puts("\nProcessed pixels: ");
    print_rendered_pixels();
    uart_puts(" | ");
    print_pixel_diff(prev_pixels, "[DRAWN ITEM]");
    draw_placement_boxes(map->items, map->num_items, CORRECT_BOX);
  }
}

void toggle_collision_debugger() {
  collision_debugger = !collision_debugger;

  if (collision_debugger) {
    render_boundaries(map->boundaries, map->num_boundaries);
    draw_items();
  } else {
    long long prev_pixels = get_rendered_pixels();

    draw_rect_from_bitmap(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, map->bitmap);
    uart_puts("\nProcessed pixels: ");
    print_rendered_pixels();
    uart_puts(" | ");
    print_pixel_diff(prev_pixels, "[DRAWN MAP]");

    draw_player();
    draw_guard(guard1, background_guard1_cache_buffer, guard1_sprite_buffer);
    draw_guard(guard2, background_guard2_cache_buffer, guard2_sprite_buffer);

    draw_time();
    draw_inventory(selected_item);
    draw_placement_boxes(map->items, map->num_items, EMPTY_BOX);
    update_placement_boxes(player->position, map->items, map->num_items);
  }
}

int get_collision_debugger_status() { return collision_debugger; }