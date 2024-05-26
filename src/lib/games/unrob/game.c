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
#include "../engine/guard.h"
#include "../engine/item.h"
#include "../engine/map-bitmap.h"
#include "../engine/player.h"
#include "maps.h"

GameMap *map = &map1;
int is_game_over = 0;
int timer_counter = 0;

unsigned int unrob_numobjs = 0;
struct Object unrob_objects[MAX_GENGINE_ENTITIES];
struct Object *player;
struct Object *guard1;
struct Object *guard2;

Bitmap background_cache_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];
Bitmap player_sprite_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];

Bitmap background_guard_1_cache_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];
Bitmap guard_1_sprite_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];

Bitmap background_guard_2_cache_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];
Bitmap guard_2_sprite_buffer[PLAYER_WIDTH * PLAYER_HEIGHT];

void initialize_game() {
  unrob_objects[unrob_numobjs].position.x = map->spawn_point.x;
  unrob_objects[unrob_numobjs].position.y = map->spawn_point.y;
  unrob_objects[unrob_numobjs].width = PLAYER_WIDTH;
  unrob_objects[unrob_numobjs].height = PLAYER_HEIGHT;
  player = &unrob_objects[unrob_numobjs];
  unrob_numobjs++;

  unrob_objects[unrob_numobjs].position.x = map->guards[0].spawn_point.x;
  unrob_objects[unrob_numobjs].position.y = map->guards[0].spawn_point.y;
  unrob_objects[unrob_numobjs].width = PLAYER_WIDTH;
  unrob_objects[unrob_numobjs].height = PLAYER_HEIGHT;
  guard1 = &unrob_objects[unrob_numobjs];
  unrob_numobjs++;

  unrob_objects[unrob_numobjs].position.x = map->guards[1].spawn_point.x;
  unrob_objects[unrob_numobjs].position.y = map->guards[1].spawn_point.y;
  unrob_objects[unrob_numobjs].width = PLAYER_WIDTH;
  unrob_objects[unrob_numobjs].height = PLAYER_HEIGHT;
  guard2 = &unrob_objects[unrob_numobjs];
  unrob_numobjs++;

  for (int i = 0; i < map->num_items; i++) {
    map->items[i].entity.position = map->spawn_point;
  }

  copy_rect(0, 0, 0, 0, PLAYER_WIDTH, PLAYER_WIDTH, PLAYER_HEIGHT,
            get_player_sprite(), player_sprite_buffer);

  copy_rect(0, 0, 0, 0, PLAYER_WIDTH, PLAYER_WIDTH, PLAYER_HEIGHT,
            get_guard_sprite(map->guards[0].direction), guard_1_sprite_buffer);
  copy_rect(0, 0, 0, 0, PLAYER_WIDTH, PLAYER_WIDTH, PLAYER_HEIGHT,
            get_guard_sprite(map->guards[1].direction), guard_2_sprite_buffer);
}

int selected_item = 0;
unsigned int game_time = 0;
unsigned int game_score = 0;
char *game_time_str = "Time: 0:00";
char *game_score_str = "Score: 000";

void initialize_buffers() {
  // reset the rendered pixels
  clear_frame_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);

  long long prev_pixels = get_rendered_pixels();

  uart_puts("\n\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[RESET RENDERED PIXELS]");

  // Display the map
  draw_rect_from_bitmap_alpha(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, map->bitmap,
                              50);

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
  copy_rect(map->spawn_point.x, map->spawn_point.y, 0, 0, SCREEN_WIDTH,
            PLAYER_WIDTH, PLAYER_HEIGHT, map->bitmap, background_cache_buffer);

  copy_rect(map->guards[0].spawn_point.x, map->guards[0].spawn_point.y, 0, 0,
            SCREEN_WIDTH, PLAYER_WIDTH, PLAYER_HEIGHT, map->bitmap,
            background_guard_1_cache_buffer);

  copy_rect(map->guards[1].spawn_point.x, map->guards[1].spawn_point.y, 0, 0,
            SCREEN_WIDTH, PLAYER_WIDTH, PLAYER_HEIGHT, map->bitmap,
            background_guard_2_cache_buffer);
}

void draw_guard(Guard *guard, Bitmap *guard_bg_cache_buffer,
                Bitmap *guard_sprite_buffer) {
  if (!guard)
    return; // Ensure guard object exists

  long long prev_pixels = get_rendered_pixels();

  // Copy the background to the guard's position
  copy_rect(guard->entity.position.x, guard->entity.position.y, 0, 0,
            SCREEN_WIDTH, PLAYER_WIDTH, PLAYER_HEIGHT, map->bitmap,
            guard_bg_cache_buffer);

  // Draw the guard sprite
  draw_transparent_image(guard->entity.position.x, guard->entity.position.y,
                         PLAYER_WIDTH, PLAYER_HEIGHT, guard_sprite_buffer);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[DRAWN GUARD]");
}

void move_guard(Guard *guard, Bitmap *guard_sprite_buffer,
                Bitmap *guard_bg_cache_buffer) {
  int force_redraw = false;
  // Bitmap *guard_sprite = get_guard_sprite(guard->direction);
  guard_sprite_buffer = get_guard_sprite(guard->direction);
  switch (guard->direction) {
  case UP:
    if (guard->direction != UP) {
      guard->direction = UP;
      force_redraw = true;
    }
    break;
  case DOWN:
    if (guard->direction != DOWN) {
      guard->direction = DOWN;
      force_redraw = true;
    }
    break;
  case LEFT:
    if (guard->direction != LEFT) {
      guard->direction = LEFT;
      force_redraw = true;
    }
    break;
  case RIGHT:
    if (guard->direction != RIGHT) {
      guard->direction = RIGHT;
      force_redraw = true;
    }
    break;
  default:
    return; // Do nothing if another key is pressed
  }

  // if (force_redraw)
  // copy_rect(0, 0, 0, 0, GUARD_WIDTH, GUARD_WIDTH, GUARD_HEIGHT,
  //           get_guard_sprite(), guard_sprite_buffer);

  move_in_boundaries_guard(map->boundaries, map->num_boundaries, &guard->direction,
                     &guard->entity.position, map->bitmap,
                     guard_bg_cache_buffer, guard_sprite_buffer, force_redraw);

  Position player_bottom_right = {
      .x = player->position.x + PLAYER_WIDTH,
      .y = player->position.y + PLAYER_HEIGHT,
  };

  Position guard_bottom_right = {
      .x = guard->entity.position.x + PLAYER_WIDTH,
      .y = guard->entity.position.y + PLAYER_HEIGHT,
  };
  if(is_intersect_guard( &guard->entity.position, &guard_bottom_right, &player->position, &player_bottom_right)){
    game_over();
  }
}

void start_unrob_game() {
  is_game_over = 0;
  timer_counter = 0;
  // turn off debugger upon game start
  collision_debugger = 0;
  reset_rendered_pixels();

  initialize_game();
  initialize_buffers();

  draw_player();
  draw_guard(&map->guards[0], background_guard_1_cache_buffer,
             guard_1_sprite_buffer);
  draw_guard(&map->guards[1], background_guard_2_cache_buffer,
             guard_2_sprite_buffer);

  draw_inventory(selected_item);
  draw_placement_boxes(map->items, map->num_items, EMPTY_BOX);

  game_time = 61;
  draw_time();
  sys_timer3_init();
  sys_timer3_irq_enable();

  draw_score();
}

void countdown(void) {
  timer_counter++;
  uart_puts("\n\nTimer Counter: ");
  uart_dec(timer_counter);
  uart_puts("\n");
  if (game_time) {
    if(timer_counter >= 2){
      timer_counter = 0;
      game_time--;
      draw_time();

    }
    move_guard(&map->guards[0], guard_1_sprite_buffer,
               background_guard_1_cache_buffer);
    move_guard(&map->guards[1], guard_2_sprite_buffer,
               background_guard_2_cache_buffer);
  } else {
    game_over();
  }
}

void game_over(){
    clear_frame_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);
    draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00000000, 1);
    draw_string(SCREEN_WIDTH / 2 - 5 * FONT_WIDTH * GENGINE_TIME_ZOOM,
                SCREEN_HEIGHT / 2 - FONT_HEIGHT * GENGINE_TIME_ZOOM, "GAME OVER",
                0x00FF0000, GENGINE_TIME_ZOOM);
    sys_timer3_irq_disable();
    is_game_over = 1;
}

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

const Bitmap *get_guard_sprite(enum Direction direction) {
  switch (direction) {
  case UP:
    return guard_up;
  case DOWN:
    return guard_down;
  case LEFT:
    return guard_left;
  case RIGHT:
    return guard_right;
  default:
    return guard_up;
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

  // Draw the player sprite
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
  if(is_game_over){
    return;
  }
  if (!player)
    return; // Ensure player object exists

  int force_redraw = false;
  Position player_bottom_right = {
      .x = player->position.x + PLAYER_WIDTH,
      .y = player->position.y + PLAYER_HEIGHT,
  };

  Position guard1_bottom_right = {
      .x = map->guards[0].entity.position.x + PLAYER_WIDTH,
      .y = map->guards[0].entity.position.y + PLAYER_HEIGHT,
  };

  Position guard2_bottom_right = {
      .x = map->guards[1].entity.position.x + PLAYER_WIDTH,
      .y = map->guards[1].entity.position.y + PLAYER_HEIGHT,
  };

  if(is_intersect_guard(&player->position, &player_bottom_right, &map->guards[0].entity.position, &guard1_bottom_right)){
    game_over();
  }
  if(is_intersect_guard(&player->position, &player_bottom_right, &map->guards[1].entity.position, &guard2_bottom_right)){
    game_over();
  }

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

  // Display position change
  uart_puts("\n\nReceived key: ");
  uart_puts(COLOR.TEXT.BLUE);
  char2upper(&key);
  uart_sendc(key);
  uart_puts(COLOR.RESET);

  if (force_redraw)
    copy_rect(0, 0, 0, 0, PLAYER_WIDTH, PLAYER_WIDTH, PLAYER_HEIGHT,
              get_player_sprite(), player_sprite_buffer);

  move_in_boundaries(map->boundaries, map->num_boundaries, player_direction,
                     &player->position, map->bitmap, background_cache_buffer,
                     player_sprite_buffer, force_redraw, false);
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
    draw_guard(&map->guards[0], background_guard_1_cache_buffer,
               guard_1_sprite_buffer);
    draw_guard(&map->guards[1], background_guard_2_cache_buffer,
               guard_2_sprite_buffer);

    draw_time();
    draw_inventory(selected_item);
    draw_placement_boxes(map->items, map->num_items, EMPTY_BOX);
    update_placement_boxes(player->position, map->items, map->num_items);
  }
}

int get_collision_debugger_status() { return collision_debugger; }

//write me a function to check if player intersect with a guard
//the parameter are Position a, Position b, Position c, Position d
//a is the top right of the player, b is the bottom right of the player
//c is the top right of the guard, d is the bottom right of the guard
//return 1 if intersect, 0 if not
int is_intersect_guard(const Position *a, const Position *b, const Position *c,
                 const Position *d) {
//check if any point of the player is inside the guard
  if((a->x >= c->x && a->x <= d->x) && (a->y >= c->y && a->y <= d->y)){
    return 1;
  }
  if((b->x >= c->x && b->x <= d->x) && (b->y >= c->y && b->y <= d->y)){
    return 1;
  }
  if((a->x >= c->x && a->x <= d->x) && (b->y >= c->y && b->y <= d->y)){
    return 1;
  }
  if((b->x >= c->x && b->x <= d->x) && (a->y >= c->y && a->y <= d->y)){
    return 1;
  }
  //check if any point of the guard is inside the player
  if((c->x >= a->x && c->x <= b->x) && (c->y >= a->y && c->y <= b->y)){
    return 1;
  }
  if((d->x >= a->x && d->x <= b->x) && (d->y >= a->y && d->y <= b->y)){
    return 1;
  }
  if((c->x >= a->x && c->x <= b->x) && (d->y >= a->y && d->y <= b->y)){
    return 1;
  }
  if((d->x >= a->x && d->x <= b->x) && (c->y >= a->y && c->y <= b->y)){
    return 1;
  }
  return 0;
}