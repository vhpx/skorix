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
#include "../screens/game-menu.h"
#include "../screens/post-game-buttons.h"
#include "../screens/post-game.h"
#include "../screens/stage-select.h"

GameMap *map;

int enable_game_debugger = false;
const int SKIP_STAGE_ANIMATION = true;

int is_game_over = false;
int is_game_start = false;

int select_game_option = 1;
int is_level_selected = false;

int selected_level = 1;
int selected_item = 0;

unsigned int interval = 0;
unsigned int time_passed = 0;
int timer_counter = 0;

unsigned int game_time = 0;
unsigned int game_score = 0;

char *game_time_str = "Time: 0:00";
char *game_score_str = "Score: 000";

void initialize_game() {
  map->player.position = map->spawn_point;

  for (int i = 0; i < map->num_guards; i++) {
    map->guards[i].entity.position = map->guards[i].spawn_point;
  }

  // shuffle items
  shuffleItems(map->items, map->num_items, time_passed);

  for (int i = 0; i < map->num_items; i++) {
    if (map->items[i].final_position.x == -1 &&
        map->items[i].final_position.y == -1)
      continue; // Skip if the item has no final position

    map->items[i].entity.position = map->player.position;
    map->items[i].entity.background_cache = 0;
  }

  copy_rect(0, 0, 0, 0, PLAYER_WIDTH, PLAYER_WIDTH, PLAYER_HEIGHT,
            get_player_sprite(), map->player.sprite);

  for (int i = 0; i < map->num_guards; i++) {
    copy_rect(0, 0, 0, 0, PLAYER_WIDTH, PLAYER_WIDTH, PLAYER_HEIGHT,
              get_guard_sprite(map->guards[i].direction),
              map->guards[i].entity.sprite);
  }
}

void initialize_buffers() {
  // reset the rendered pixels
  clear_frame_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);

  long long prev_pixels = get_rendered_pixels();

  uart_puts("\n");
  print_rendered_pixels(true);
  print_pixel_diff(prev_pixels, "[RESET RENDERED PIXELS]");

  if (SKIP_STAGE_ANIMATION) {
    draw_rect_from_bitmap(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, map->bitmap);
  } else {
    // Display the map
    draw_rect_from_bitmap_alpha(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, map->bitmap,
                                50);

    print_rendered_pixels(true);
    print_pixel_diff(prev_pixels, "[DRAWN INITIAL MAP] ALPHA: 50");

    move_items_to_final_position();
    // move_items_to_final_position(temp_items, map->num_items, map->bitmap);
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
                                  map->bitmap, i);
      print_rendered_pixels(true);
      print_pixel_diff(prev_pixels, msg);
      wait_msec(200);
    }
  }

  // Reset all item placement positions
  for (int i = 0; i < map->num_items; i++) {
    map->items[i].entity.position.x = -1;
    map->items[i].entity.position.y = -1;
  }

  // Reset all guard positions
  for (int i = 0; i < map->num_guards; i++) {
    map->guards[i].entity.position = map->guards[i].spawn_point;
  }

  // Reset the player position
  map->player.position.x = map->spawn_point.x;
  map->player.position.y = map->spawn_point.y;

  // Copy the initial portion of the background to the cache buffer
  copy_rect(map->player.position.x, map->player.position.y, 0, 0, SCREEN_WIDTH,
            PLAYER_WIDTH, PLAYER_HEIGHT, map->bitmap,
            map->player.background_cache);

  for (int i = 0; i < map->num_guards; i++) {
    copy_rect(map->guards[i].entity.position.x,
              map->guards[i].entity.position.y, 0, 0, SCREEN_WIDTH,
              PLAYER_WIDTH, PLAYER_HEIGHT, map->bitmap,
              map->guards[i].entity.background_cache);
  }
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

  print_rendered_pixels(true);
  print_pixel_diff(prev_pixels, "[DRAWN GUARD]");
}

void move_guard(Guard *guard, const Bitmap *guard_sprite_buffer,
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

  move_guard_in_boundaries(map->boundaries, map->num_boundaries,
                           &guard->direction, &guard->entity.position,
                           map->bitmap, guard_bg_cache_buffer,
                           guard_sprite_buffer, force_redraw);

  Position player_bottom_right = {
      .x = map->player.position.x + PLAYER_WIDTH,
      .y = map->player.position.y + PLAYER_HEIGHT,
  };

  Position guard_bottom_right = {
      .x = guard->entity.position.x + PLAYER_WIDTH,
      .y = guard->entity.position.y + PLAYER_HEIGHT,
  };
  if (is_intersect_guard(&guard->entity.position, &guard_bottom_right,
                         &map->player.position, &player_bottom_right)) {
    game_over();
  }
}

// function to start the game
void game_start_selector() {
  draw_image(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, game_menu);
  draw_transparent_image(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 150, 220, 70,
                         button_exit);
  draw_transparent_image(SCREEN_WIDTH / 2 - 135, SCREEN_HEIGHT - 280, 290, 100,
                         button_start_selected);
  select_game_option = 1;
}

// function to start or exit game
void select_game_start_exit(char key) {
  draw_image(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, game_menu);
  switch (key) {
  case 'w':
    // draw the game_home_screen_buffer
    draw_transparent_image(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 150, 220, 70,
                           button_exit);
    draw_transparent_image(SCREEN_WIDTH / 2 - 135, SCREEN_HEIGHT - 280, 290,
                           100, button_start_selected);
    select_game_option = 1;
    break;
  case 's':
    // draw_rect_from_bitmap(0, 0, 290, 360, game_home_screen_buffer);
    draw_transparent_image(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 250, 220, 70,
                           button_start);
    draw_transparent_image(SCREEN_WIDTH / 2 - 135, SCREEN_HEIGHT - 180, 290,
                           100, button_exit_selected);
    select_game_option = 0;
    break;
  default:
    return;
  }
}

// default level selector
void level_selector() { draw_level_selection_base(1); }

// function to select the level
void select_level(char key) {
  switch (key) {
  case 'w':
    if (selected_level > 1) {
      selected_level--;
    }
    draw_level_selection_base(selected_level);
    break;
  case 's':
    if (selected_level < 3) {
      selected_level++;
    }
    draw_level_selection_base(selected_level);
    break;
  case '\n':
    uart_puts("\n\nSelected Level: ");
    uart_dec(selected_level);

    if (enable_rendering_debugger)
      uart_puts("\n\n");

    select_map(selected_level);
    start_unrob_game();

    break;
  default:
    return;
  }
}

// draw the level selection base like pointing to level 1, level 2, level 3
void draw_level_selection_base(int selected_level) {
  // black screen
  draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00000000, 1);

  // LEVELS
  draw_string(SCREEN_WIDTH / 2 - 5 * FONT_WIDTH * GENGINE_TIME_ZOOM,
              SCREEN_HEIGHT / 2 - FONT_HEIGHT * GENGINE_TIME_ZOOM, "LEVELS",
              0x00FF0000, GENGINE_TIME_ZOOM);
  draw_string(SCREEN_WIDTH / 2 - 5 * FONT_WIDTH * GENGINE_TIME_ZOOM,
              SCREEN_HEIGHT / 2 + FONT_HEIGHT * GENGINE_TIME_ZOOM, "1. Level 1",
              0x0000FF00, GENGINE_TIME_ZOOM);
  draw_string(SCREEN_WIDTH / 2 - 5 * FONT_WIDTH * GENGINE_TIME_ZOOM,
              SCREEN_HEIGHT / 2 + 2 * FONT_HEIGHT * GENGINE_TIME_ZOOM,
              "2. Level 2", 0x00FFFF00, GENGINE_TIME_ZOOM);
  draw_string(SCREEN_WIDTH / 2 - 5 * FONT_WIDTH * GENGINE_TIME_ZOOM,
              SCREEN_HEIGHT / 2 + 3 * FONT_HEIGHT * GENGINE_TIME_ZOOM,
              "3. Level 3", 0x00FF00FF, GENGINE_TIME_ZOOM);

  // draw_rect at level selected
  if (selected_level == 1) {
    draw_string(SCREEN_WIDTH / 2 - 5 * FONT_WIDTH * GENGINE_TIME_ZOOM - 50,
                SCREEN_HEIGHT / 2 + FONT_HEIGHT * GENGINE_TIME_ZOOM, ">",
                0x00FF0000, GENGINE_TIME_ZOOM);
  } else if (selected_level == 2) {
    draw_string(SCREEN_WIDTH / 2 - 5 * FONT_WIDTH * GENGINE_TIME_ZOOM - 50,
                SCREEN_HEIGHT / 2 + 2 * FONT_HEIGHT * GENGINE_TIME_ZOOM, ">",
                0x00FF0000, GENGINE_TIME_ZOOM);
  } else if (selected_level == 3) {
    draw_string(SCREEN_WIDTH / 2 - 5 * FONT_WIDTH * GENGINE_TIME_ZOOM - 50,
                SCREEN_HEIGHT / 2 + 3 * FONT_HEIGHT * GENGINE_TIME_ZOOM, ">",
                0x00FF0000, GENGINE_TIME_ZOOM);
  }
}

void select_map(int map_num) {
  switch (map_num) {
  case 1:
    map = &map1;
    break;
  case 2:
    map = &map2;
    break;
  case 3:
    map = &map3;
    break;
  default:
    map = &map1;
    break;
  }
}

void start_unrob_game() {
  uart_puts("\n\nStarting Unrob Game...");

  // turn off debugger upon game start
  is_game_over = false;
  enable_game_debugger = false;

  reset_rendered_pixels();

  initialize_game();
  initialize_buffers();

  draw_player();

  for (int i = 0; i < map->num_guards; i++) {
    draw_guard(&map->guards[i], map->guards[i].entity.background_cache,
               map->guards[i].entity.sprite);
  }

  display_selected_item(selected_item, map->items, map->num_items);
  draw_placement_boxes(map->items, map->num_items, EMPTY_BOX);

  game_time = 60;

  sys_timer3_init();
  sys_timer3_irq_enable();

  draw_time();
  draw_score();
}

void countdown(void) {
  // This function is called every 1/5 second
  time_passed++;

  if (interval < 20) {
    // If less than a full second (20 intervals of 1/5 second each) has passed,
    // increment the interval by 8. This means that a "second" in this context
    // will pass in less than 3 intervals (24/8), which is less than half a real
    // second.
    interval += 8;
  } else {
    // If a full "second" (20 intervals) or more has passed, increment the timer
    // counter. This counter can be used to track the number of full "seconds"
    // that have passed.
    timer_counter++;

    // If the rendering debugger is enabled, print the current value of the
    // timer counter to the UART. This can be useful for debugging
    // timing-related issues.
    if (enable_rendering_debugger) {
      uart_puts("\n\nTimer Counter: ");
      uart_dec(timer_counter);
      uart_puts("\n");
    }

    // Subtract 20 from the interval, effectively "consuming" the full "second"
    // that just passed. If the interval was more than 20, the excess will carry
    // over to the next "second".
    interval = interval - 20;
  }

  if (game_time) {
    if (is_level_selected) { // check if game start
      if (timer_counter >= 2) {
        timer_counter = 0;
        game_time--;
        draw_time();
      }

      for (int i = 0; i < map->num_guards; i++) {
        move_guard(&map->guards[i], map->guards[i].entity.sprite,
                   map->guards[i].entity.background_cache);
      }
    }
  } else {
    game_over();
  }
}

void game_over() {
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
    if (map->items[i].final_position.x == -1 &&
        map->items[i].final_position.y == -1)
      continue; // Skip if the item has no final position

    float time = 0.0f;
    float speed = 0.005f; // Adjust this value to control the speed of movement

    const Item item = map->items[i];
    const Position final_position = item.final_position;
    const Size size = item.entity.size;

    Position current_position = item.entity.position;

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
                             size.height, item.entity.sprite);

      // Update the item's position
      current_position = new_position;
      map->items[i].entity.position = new_position;

      // Increase the time
      time += speed;

      // Wait for a short period of time before the next update
      wait_msec(10);

      print_rendered_pixels(true);
      print_pixel_diff(prev_pixels, msg);
    }
  }
}

void execute_main_action() {
  Item *items = map->items;
  int num_items = map->num_items;

  int nearest_box_index =
      get_nearest_box_index(&map->player.position, items, num_items);

  // If no activated box is found within GENGINE_PLACEMENT_RANGE, return
  if (nearest_box_index == -1) {
    uart_puts(COLOR.TEXT.RED);
    uart_puts("\n\nNo activated box found within placement range.");
    uart_puts(COLOR.RESET);
    return;
  }

  int is_empty = is_box_empty(items, num_items, nearest_box_index);
  enum Action action = are_all_items_placed(items, num_items)  ? PICK_UP
                       : is_item_placed(&items[selected_item]) ? NO_ACTION
                       : is_empty                              ? PLACE_DOWN
                                                               : SWAP_ITEM;

  // If no activated box is found within GENGINE_PLACEMENT_RANGE, return
  if (action == NO_ACTION) {
    uart_puts(COLOR.TEXT.RED);
    uart_puts("\n\nSelected item is already placed.");
    uart_puts(COLOR.RESET);
    return;
  }

  uart_puts("\n\nAction: ");
  uart_puts(COLOR.TEXT.BLUE);
  uart_puts(action == PICK_UP      ? "PICK UP"
            : action == PLACE_DOWN ? "PLACE DOWN"
                                   : "SWAP ITEM");
  uart_puts(COLOR.RESET);

  switch (action) {
  case PICK_UP:
    pick_up_item(items, num_items, nearest_box_index);
    break;

  case PLACE_DOWN:
    items[selected_item].entity.position =
        items[nearest_box_index].final_position;

    if (selected_item == nearest_box_index) {
      draw_item_with_box(&items[nearest_box_index], CORRECT_BOX);
      game_score += 10;
      draw_score();
    } else {
      draw_item_with_box(&items[selected_item], INCORRECT_BOX);
    }
    break;

  case SWAP_ITEM:
    swap_items_in_box(items, num_items, nearest_box_index,
                      &items[selected_item]);
    if (selected_item == items[nearest_box_index].id) {
      draw_item_with_box(&items[nearest_box_index], CORRECT_BOX);
      game_score += 10;
      draw_score();
    } else {
      draw_item_with_box(&items[nearest_box_index], INCORRECT_BOX);
    }
    break;

  default:
    break;
  }

  update_placement_boxes(map->player.position, items, num_items);
  display_selected_item(selected_item, map->items, map->num_items);
}

void draw_item_with_box(Item *item, enum Box box) {
  draw_placement_boxes(item, 1, box);
}

void update_placement_boxes(Position position, Item *items, int num_items) {
  int nearest_box_index = get_nearest_box_index(&position, items, num_items);

  for (int i = 0; i < num_items; i++) {
    if (enable_game_debugger || is_item_in_correct_position(&items[i])) {
      if (enable_game_debugger) {
        Item item = items[i];
        item.entity.position = item.final_position;
        draw_placement_boxes(&item, 1, CORRECT_BOX);
      } else
        draw_placement_boxes(&items[i], 1, CORRECT_BOX);
    } else if (!is_box_empty(items, num_items, i)) {
      draw_placement_boxes(&items[i], 1, INCORRECT_BOX);
    } else if (i == nearest_box_index) {
      draw_placement_boxes(&items[i], 1, IN_RANGE_BOX);
    } else {
      draw_placement_boxes(&items[i], 1, EMPTY_BOX);
    }
  }
}

unsigned int get_placement_box_color(enum Box box) {
  switch (box) {
  case EMPTY_BOX:
    return 0xFFFFFF; // White
  case INCORRECT_BOX:
    return 0xFF0000; // Red
  case CORRECT_BOX:
    return 0x00FF00; // Green
  case IN_RANGE_BOX:
    return 0xFFFF00; // Yellow
  default:
    return 0xFFFFFF; // Default to white
  }
}

void draw_placement_boxes(Item *items, int num_items, enum Box box) {
  unsigned int color = get_placement_box_color(box);

  for (int i = 0; i < num_items; i++) {

    long long prev_pixels = get_rendered_pixels();
    char msg[MAX_STR_LENGTH];
    clrstr(msg);

    append_str(msg, "[DRAWN PLACE DOWN BOX] Name: ");
    append_str(msg, items[i].name);

    int x = items[i].final_position.x;
    int y = items[i].final_position.y;

    int width = GENGINE_ITEM_SIZE;
    int height = GENGINE_ITEM_SIZE;

    if (items[i].entity.background_cache == 0)
      copy_rect(items[i].entity.position.x, items[i].entity.position.y, 0, 0,
                SCREEN_WIDTH, width, height, map->bitmap,
                items[i].entity.background_cache);

    // Draw the background from the background_cache
    if (items[i].entity.position.x != -1 && items[i].entity.position.y != -1)
      draw_image(items[i].entity.position.x, items[i].entity.position.y, width,
                 height, items[i].entity.background_cache);

    // Draw the item
    if (items[i].entity.position.x != -1 && items[i].entity.position.y != -1)
      draw_transparent_image(items[i].entity.position.x,
                             items[i].entity.position.y, width, height,
                             items[i].entity.sprite);

    // Draw player
    draw_player();

    if (items[i].final_position.x == -1 && items[i].final_position.y == -1)
      continue; // Skip if the item has no final position

    // Draw the box using lines
    draw_line(x, y + 2, x + width, y + 2, color, 4);           // Top line
    draw_line(x + 2, y, x + 2, y + height, color, 4);          // Left line
    draw_line(x + width, y, x + width, y + height, color, 4);  // Right line
    draw_line(x, y + height, x + width, y + height, color, 4); // Bottom line

    print_rendered_pixels(true);
    print_pixel_diff(prev_pixels, msg);
  }
}

void draw_player() {
  long long prev_pixels = get_rendered_pixels();

  // Copy the background to the player's position
  copy_rect(map->player.position.x, map->player.position.y, 0, 0, SCREEN_WIDTH,
            PLAYER_WIDTH, PLAYER_HEIGHT, map->bitmap,
            map->player.background_cache);

  // Draw the player sprite
  draw_transparent_image(map->player.position.x, map->player.position.y,
                         PLAYER_WIDTH, PLAYER_HEIGHT, map->player.sprite);

  print_rendered_pixels(true);
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

  print_rendered_pixels(true);
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

  print_rendered_pixels(true);
  print_pixel_diff(prev_pixels, "[DRAWN SCORE]");
}

void move_player(char key) {
  if (is_game_over) {
    game_over();
    return;
  }

  int force_redraw = false;

  Position player_bottom_right = {
      .x = map->player.position.x + PLAYER_WIDTH,
      .y = map->player.position.y + PLAYER_HEIGHT,
  };

  Position guard1_bottom_right = {
      .x = map->guards[0].entity.position.x + PLAYER_WIDTH,
      .y = map->guards[0].entity.position.y + PLAYER_HEIGHT,
  };

  Position guard2_bottom_right = {
      .x = map->guards[1].entity.position.x + PLAYER_WIDTH,
      .y = map->guards[1].entity.position.y + PLAYER_HEIGHT,
  };

  if (is_intersect_guard(&map->player.position, &player_bottom_right,
                         &map->guards[0].entity.position,
                         &guard1_bottom_right)) {
    game_over();
  }
  if (is_intersect_guard(&map->player.position, &player_bottom_right,
                         &map->guards[1].entity.position,
                         &guard2_bottom_right)) {
    game_over();
  }

  // display all item positions in console for debugging
  // for (int i = 0; i < map->num_items; i++) {
  //   uart_puts("\n\nItem: ");
  //   uart_puts(COLOR.TEXT.BLUE);
  //   uart_puts(map->items[i].name);
  //   uart_puts(COLOR.RESET);

  //   uart_puts("\nPosition: ");
  //   uart_puts(COLOR.TEXT.BLUE);
  //   uart_dec(map->items[i].entity.position.x);
  //   uart_puts(", ");
  //   uart_dec(map->items[i].entity.position.y);
  //   uart_puts(COLOR.RESET);
  // }

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
              get_player_sprite(), map->player.sprite);

  move_in_boundaries(map->boundaries, map->num_boundaries, player_direction,
                     &map->player.position, map->bitmap,
                     map->player.background_cache, map->player.sprite,
                     force_redraw, false);
  update_placement_boxes(map->player.position, map->items, map->num_items);
}

void switch_inventory_item(char key) {
  switch (key) {
  case 'q': // Rotate left
    selected_item--;

    // Wrap around if the index goes below 0
    if (selected_item < 0) {
      selected_item = map->num_items - 1;
    }

    uart_puts("\n\nNavigating inventory item in reverse.");
    uart_puts("\nSelected Item: ");
    uart_puts(COLOR.TEXT.BLUE);
    uart_puts(map->items[selected_item].name);
    uart_puts(COLOR.RESET);

    break;

  case 'e': // Rotate right
    selected_item++;

    // Wrap around if the index exceeds array length
    if (selected_item >= map->num_items) {
      selected_item = 0;
    }

    uart_puts("\n\nNavigating inventory item forward.");
    uart_puts("\nSelected Item: ");
    uart_puts(COLOR.TEXT.BLUE);
    uart_puts(map->items[selected_item].name);
    uart_puts(COLOR.RESET);

    break;

  default:
    return; // Do nothing if another key is pressed
  }

  // Display the selected item in the inventory
  display_selected_item(selected_item, map->items, map->num_items);
}

void display_selected_item(int selected_item, Item *items, int num_items) {
  long long prev_pixels = get_rendered_pixels();

  // display top right corner
  // Draw a larger black rectangle
  draw_rect(0, 0, 54, 54, 0x00000000, 1);

  // Draw a smaller white rectangle inside the black one to create a border
  // effect
  draw_rect(5, 5, 50, 50, 0x00ffffff, 1);

  // Check if the selected item is already placed
  int is_placed = is_item_placed(&items[selected_item]);

  draw_transparent_image(8, 8, GENGINE_ITEM_SIZE, GENGINE_ITEM_SIZE,
                         items[selected_item].entity.sprite);

  // If the item is placed, draw a red crossed line
  if (is_placed) {
    draw_line(10, 8, 8 + GENGINE_ITEM_SIZE, 8 + GENGINE_ITEM_SIZE, 0x00ff0000,
              4); // Diagonal from top-left to bottom-right
    draw_line(10, 8 + GENGINE_ITEM_SIZE, 8 + GENGINE_ITEM_SIZE, 8, 0x00ff0000,
              4); // Diagonal from bottom-left to top-right
  }

  print_rendered_pixels(true);
  print_pixel_diff(prev_pixels, "[DRAWN INVENTORY]");

  // dispay on top of the player
  //  draw_rect(map->player.position.x-10, map->player.position.y-50, 110,110,
  //  0x00ffffff, 1); draw_transparent_image(map->player.position.x,
  //  map->player.position.y - 50, GENGINE_ITEM_SIZE, GENGINE_ITEM_SIZE,
  //  item_m1_allArray[selected_item]);
}

void draw_final_items() {
  long long prev_pixels;

  for (int i = 0; i < map->num_items; i++) {
    if (map->items[i].final_position.x == -1 &&
        map->items[i].final_position.y == -1)
      continue; // Skip if the item has no final position

    prev_pixels = get_rendered_pixels();

    draw_transparent_image(map->items[i].final_position.x,
                           map->items[i].final_position.y, GENGINE_ITEM_SIZE,
                           GENGINE_ITEM_SIZE, map->items[i].entity.sprite);
    print_rendered_pixels(true);
    print_pixel_diff(prev_pixels, "[DRAWN ITEM]");
    draw_placement_boxes(map->items, map->num_items, CORRECT_BOX);
  }
}

void toggle_rendering_debugger() {
  enable_rendering_debugger = !enable_rendering_debugger;
}

void toggle_game_debugger() {
  enable_game_debugger = !enable_game_debugger;

  if (enable_game_debugger) {
    render_boundaries(map->boundaries, map->num_boundaries);
    update_placement_boxes(map->player.position, map->items, map->num_items);
  } else {
    long long prev_pixels = get_rendered_pixels();

    draw_rect_from_bitmap(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, map->bitmap);
    print_rendered_pixels(true);
    print_pixel_diff(prev_pixels, "[DRAWN MAP]");

    draw_player();

    for (int i = 0; i < map->num_guards; i++) {
      draw_guard(&map->guards[i], map->guards[i].entity.background_cache,
                 map->guards[i].entity.sprite);
    }

    draw_time();
    display_selected_item(selected_item, map->items, map->num_items);
    draw_placement_boxes(map->items, map->num_items, EMPTY_BOX);
    update_placement_boxes(map->player.position, map->items, map->num_items);
  }
}

// check if player intersect with a guard
// the parameter are Position a, Position b, Position c, Position d
// a is the top right of the player, b is the bottom right of the player
// c is the top right of the guard, d is the bottom right of the guard
// return 1 if intersect, 0 if not
int is_intersect_guard(const Position *a, const Position *b, const Position *c,
                       const Position *d) {
  // Define the player's coordinates explicitly for clarity.
  Position player_top_left = {min(a->x, b->x), max(a->y, b->y)};
  Position player_bottom_right = {max(a->x, b->x), min(a->y, b->y)};

  // Define the guard's coordinates explicitly for clarity.
  Position guard_top_left = {min(c->x, d->x), max(c->y, d->y)};
  Position guard_bottom_right = {max(c->x, d->x), min(c->y, d->y)};

  // Check for direct overlap:
  if (player_top_left.x <= guard_bottom_right.x &&
      player_bottom_right.x >= guard_top_left.x &&
      player_top_left.y >= guard_bottom_right.y &&
      player_bottom_right.y <= guard_top_left.y) {
    return 1;
  }

  // Check for edge intersections:
  return is_intersect(a, b, c, d) ? 1 : 0;
}

void shuffleItems(Item *items, int num_items, int random_num) {
  // Create a pseudo-random sequence using the provided random number
  for (int i = num_items - 1; i > 0; i--) {
    // Use the random number to generate a pseudo-random index
    int j = random_num % (i + 1);

    // Swap items[i] and items[j]
    Position temp = items[i].final_position;
    items[i].final_position = items[j].final_position;
    items[j].final_position = temp;

    // Update the random number for the next iteration
    random_num =
        (random_num * 31) % num_items; // Ensure the index is within num_items
  }

  // for debugging or visualization
  // for (int i = 0; i < num_items; i++) {
  //     uart_puts("\n\nItem: ");
  //     uart_puts(COLOR.TEXT.BLUE);
  //     uart_puts(items[i].name);
  //     uart_puts(COLOR.RESET);

  //     uart_puts("\nPosition: ");
  //     uart_puts(COLOR.TEXT.BLUE);
  //     uart_dec(items[i].final_position.x);
  //     uart_puts(", ");
  //     uart_dec(items[i].final_position.y);
  //     uart_puts(COLOR.RESET);
  // }
}