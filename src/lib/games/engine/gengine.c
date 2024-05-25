// gengine.c
#include "../../headers/gengine.h"
#include "../../headers/color.h"
#include "../../headers/framebf.h"
#include "../../headers/math.h"
#include "../../headers/print.h"
#include "../../headers/string.h"
#include "../../headers/uart0.h"
#include "guard.h"

void render_boundary(Position *position, int num_positions) {
  for (int i = 0; i < num_positions; i++) {
    draw_line(position[i].x, position[i].y, position[(i + 1) % num_positions].x,
              position[(i + 1) % num_positions].y, 0xFF0000, 4);
  }
}

void render_boundaries(const Boundary *boundaries, int num_boundaries) {
  long long prev_pixels = get_rendered_pixels();

  for (int i = 0; i < num_boundaries; i++) {
    for (int j = 0; j < boundaries[i].num_positions; j++) {
      draw_line(
          boundaries[i].positions[j].x, boundaries[i].positions[j].y,
          boundaries[i].positions[(j + 1) % boundaries[i].num_positions].x,
          boundaries[i].positions[(j + 1) % boundaries[i].num_positions].y,
          0xFF0000, 4);
    }
  }

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");
  print_pixel_diff(prev_pixels, "[DRAWN COLLISION BOUNDARIES]");
}

// Function to find the orientation of ordered triplet (p, q, r).
// The function returns the following values:
// 0 --> p, q, and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(const Position *p, const Position *q, const Position *r) {
  int val = (q->y - p->y) * (r->x - q->x) - (q->x - p->x) * (r->y - q->y);
  if (val == 0)
    return 0;               // colinear
  return (val > 0) ? 1 : 2; // clock or counterclockwise
}

// Given three colinear points p, q, r, the function checks if
// point q lies on the line segment 'pr'
int on_segment(const Position *p, const Position *q, const Position *r) {
  if (q->x <= max(p->x, r->x) && q->x >= min(p->x, r->x) &&
      q->y <= max(p->y, r->y) && q->y >= min(p->y, r->y))
    return true;
  return false;
}

// The function returns true if line segment 'p1q1' and 'p2q2' intersect.
int is_intersect(const Position *p1, const Position *q1, const Position *p2,
                 const Position *q2) {
  int o1 = orientation(p1, q1, p2);
  int o2 = orientation(p1, q1, q2);
  int o3 = orientation(p2, q2, p1);
  int o4 = orientation(p2, q2, q1);

  if (o1 != o2 && o3 != o4)
    return true;

  // Check if any of the segments intersect with the X lower end
  if (o1 == 0 && on_segment(p1, p2, q1))
    return true;
  // Check if any of the segments intersect with the Y lower end
  if (o2 == 0 && on_segment(p1, q2, q1))
    return true;
  // Check if any of the segments intersect with the X upper end
  if (o3 == 0 && on_segment(p2, p1, q2))
    return true;
  // Check if any of the segments intersect with the Y upper end
  if (o4 == 0 && on_segment(p2, q1, q2))
    return true;

  return false;
}

// int intersect_end(const Position *p1, const Position *q1, const Position *p2,
//                  const Position *q2) {
//   int o1 = orientation(p1, q1, p2);
//   int o2 = orientation(p1, q1, q2);
//   int o3 = orientation(p2, q2, p1);
//   int o4 = orientation(p2, q2, q1);

//   if (o1 != o2 && o3 != o4)
//     return true;

//   // Check if any of the segments intersect with the X lower end
//   if (o1 == 0 && on_segment(p1, p2, q1))
//     return 1;
//   // Check if any of the segments intersect with the Y lower end
//   if (o2 == 0 && on_segment(p1, q2, q1))
//     return 2;
//   // Check if any of the segments intersect with the X upper end
//   if (o3 == 0 && on_segment(p2, p1, q2))
//     return 3;
//   // Check if any of the segments intersect with the Y upper end
//   if (o4 == 0 && on_segment(p2, q1, q2))
//     return 4;

//   return false;
// }


void move_in_boundaries(const Boundary *boundaries, int num_boundaries,
                        enum Direction direction, Position *current_pos,
                        const unsigned long *game_map_bitmap,
                        unsigned long *background_cache_buffer,
                        unsigned long *player_sprite_buffer, int force_redraw,
                        int is_guard) {
  int offsetX = 0, offsetY = 0;

  switch (direction) {
  case UP:
    offsetY = -GENGINE_STEP_SIZE;
    break;
  case DOWN:
    offsetY = GENGINE_STEP_SIZE;
    break;
  case LEFT:
    offsetX = -GENGINE_STEP_SIZE;
    break;
  case RIGHT:
    offsetX = GENGINE_STEP_SIZE;
    break;
  default:
    uart_puts("\nInvalid key.");
    return; // Invalid key
  }

  Position next_pos = {.x = current_pos->x + offsetX,
                       .y = current_pos->y + offsetY};

  // Define the four corners of the player's next position
  Position next_corners[4] = {
      {.x = next_pos.x, .y = next_pos.y},
      {.x = next_pos.x + PLAYER_WIDTH, .y = next_pos.y},
      {.x = next_pos.x, .y = next_pos.y + PLAYER_HEIGHT},
      {.x = next_pos.x + PLAYER_WIDTH, .y = next_pos.y + PLAYER_HEIGHT}};

  if (is_guard == false) {
    uart_puts("\nPlayer position: ");
    uart_puts("(");
    uart_puts(COLOR.TEXT.YELLOW);
    uart_dec(current_pos->x);
    uart_puts(COLOR.RESET);
    uart_puts(", ");
    uart_puts(COLOR.TEXT.YELLOW);
    uart_dec(current_pos->y);
    uart_puts(COLOR.RESET);
    uart_puts(") -> (");
    uart_puts(COLOR.TEXT.YELLOW);
    uart_dec(next_pos.x);
    uart_puts(COLOR.RESET);
    uart_puts(", ");
    uart_puts(COLOR.TEXT.YELLOW);
    uart_dec(next_pos.y);
    uart_puts(COLOR.RESET);
    uart_puts(")\n");
  }

  // Calculate rectangular regions that need to be updated
  int erase_x = current_pos->x;
  int erase_y = current_pos->y;
  int redraw_x = next_pos.x;
  int redraw_y = next_pos.y;
  int update_width = PLAYER_WIDTH;
  int update_height = PLAYER_HEIGHT;

  // Check intersections with boundaries
  for (int i = 0; i < num_boundaries; i++) {
    for (int j = 0; j < boundaries[i].num_positions; j++) {
      const Position *boundary_start = &boundaries[i].positions[j];
      const Position *boundary_end =
          &boundaries[i].positions[(j + 1) % boundaries[i].num_positions];

      for (int k = 0; k < 4; k++) {
        if (is_intersect(current_pos, &next_corners[k], boundary_start,
                         boundary_end)) {
          if (is_guard == false) {
            uart_puts(COLOR.TEXT.RED);
            uart_puts("Intersection detected with boundary");
            uart_puts(COLOR.RESET);
            uart_puts(": (");
            uart_puts(COLOR.TEXT.YELLOW);
            uart_dec(boundary_start->x);
            uart_puts(COLOR.RESET);
            uart_puts(", ");
            uart_puts(COLOR.TEXT.YELLOW);
            uart_dec(boundary_start->y);
            uart_puts(COLOR.RESET);
            uart_puts(") -> (");
            uart_puts(COLOR.TEXT.YELLOW);
            uart_dec(boundary_end->x);
            uart_puts(COLOR.RESET);
            uart_puts(", ");
            uart_puts(COLOR.TEXT.YELLOW);
            uart_dec(boundary_end->y);
            uart_puts(COLOR.RESET);
            uart_puts(")\n");
          }

          if (force_redraw == false)
            return; // Intersection detected

          // Adjust redraw region to the intersection point
          redraw_x = current_pos->x;
          redraw_y = current_pos->y;

          long long prev_pixels = get_rendered_pixels();

          // Update only the necessary portions of the background and sprite
          draw_rect_from_bitmap(erase_x, erase_y, update_width, update_height,
                                background_cache_buffer);
          uart_puts("\nProcessed pixels: ");
          print_rendered_pixels();
          uart_puts(" | ");

          if (is_guard == false)
            print_pixel_diff(prev_pixels, "[ERASED PLAYER]");
          else
            print_pixel_diff(prev_pixels, "[ERASED GUARD]");

          prev_pixels = get_rendered_pixels();

          copy_rect(redraw_x, redraw_y, 0, 0, SCREEN_WIDTH, update_width,
                    update_height, game_map_bitmap, background_cache_buffer);
          draw_transparent_image(redraw_x, redraw_y, update_width,
                                 update_height, player_sprite_buffer);

          uart_puts("\nProcessed pixels: ");
          print_rendered_pixels();
          uart_puts(" | ");

          if (is_guard == false)
            print_pixel_diff(prev_pixels, "[DRAWN PLAYER]");
          else
            print_pixel_diff(prev_pixels, "[DRAWN GUARD]");

          return; // Intersection detected
        }
      }
    }
  }

  // Adjust regions if the player moved partially off-screen
  if (erase_x < 0) {
    erase_x = 0;
    update_width += current_pos->x;
  }

  if (erase_y < 0) {
    erase_y = 0;
    update_height += current_pos->y;
  }

  if (redraw_x + PLAYER_WIDTH > SCREEN_WIDTH) {
    update_width = SCREEN_WIDTH - redraw_x;
  }

  if (redraw_y + PLAYER_HEIGHT > SCREEN_HEIGHT) {
    update_height = SCREEN_HEIGHT - redraw_y;
  }

  long long prev_pixels = get_rendered_pixels();

  // Update only the necessary portions of the background and sprite
  draw_rect_from_bitmap(erase_x, erase_y, update_width, update_height,
                        background_cache_buffer);
  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");

  if (is_guard == false)
    print_pixel_diff(prev_pixels, "[ERASED PLAYER]");
  else
    print_pixel_diff(prev_pixels, "[ERASED GUARD]");

  prev_pixels = get_rendered_pixels();

  copy_rect(redraw_x, redraw_y, 0, 0, SCREEN_WIDTH, update_width, update_height,
            game_map_bitmap, background_cache_buffer);
  draw_transparent_image(redraw_x, redraw_y, update_width, update_height,
                         player_sprite_buffer);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");

  if (is_guard == false)
    print_pixel_diff(prev_pixels, "[DRAWN PLAYER]");
  else
    print_pixel_diff(prev_pixels, "[DRAWN GUARD]");

  // Update player's current position
  current_pos->x = next_pos.x;
  current_pos->y = next_pos.y;
}

//up: 0, down: 1, left: 2, right: 3
void move_in_boundaries_guard(const Boundary *boundaries, int num_boundaries,
                        enum Direction *direction, Position *current_pos,
                        const unsigned long *game_map_bitmap,
                        unsigned long *background_cache_buffer,
                        unsigned long *player_sprite_buffer, int force_redraw) {
  int offsetX = 0, offsetY = 0;

  switch (*direction) {
  case UP:
    offsetY = -GENGINE_STEP_SIZE;
    break;
  case DOWN:
    offsetY = GENGINE_STEP_SIZE;
    break;
  case LEFT:
    offsetX = -GENGINE_STEP_SIZE;
    break;
  case RIGHT:
    offsetX = GENGINE_STEP_SIZE;
    break;
  default:
    uart_puts("\nInvalid key.");
    return; // Invalid key
  }

  Position next_pos = {.x = current_pos->x + offsetX,
                       .y = current_pos->y + offsetY};

  // Define the four corners of the player's next position
  Position next_corners[4] = {
      {.x = next_pos.x, .y = next_pos.y},
      {.x = next_pos.x + PLAYER_WIDTH, .y = next_pos.y},
      {.x = next_pos.x, .y = next_pos.y + PLAYER_HEIGHT},
      {.x = next_pos.x + PLAYER_WIDTH, .y = next_pos.y + PLAYER_HEIGHT}};

    // uart_puts("\nGuard position: ");
    // uart_puts("(");
    // uart_puts(COLOR.TEXT.YELLOW);
    // uart_dec(current_pos->x);
    // uart_puts(COLOR.RESET);
    // uart_puts(", ");
    // uart_puts(COLOR.TEXT.YELLOW);
    // uart_dec(current_pos->y);
    // uart_puts(COLOR.RESET);
    // uart_puts(") -> (");
    // uart_puts(COLOR.TEXT.YELLOW);
    // uart_dec(next_pos.x);
    // uart_puts(COLOR.RESET);
    // uart_puts(", ");
    // uart_puts(COLOR.TEXT.YELLOW);
    // uart_dec(next_pos.y);
    // uart_puts(COLOR.RESET);
    // uart_puts(")\n");

  // Calculate rectangular regions that need to be updated
  int erase_x = current_pos->x;
  int erase_y = current_pos->y;
  int redraw_x = next_pos.x;
  int redraw_y = next_pos.y;
  int update_width = PLAYER_WIDTH;
  int update_height = PLAYER_HEIGHT;

  // Check intersections with boundaries
  for (int i = 0; i < num_boundaries; i++) {
    for (int j = 0; j < boundaries[i].num_positions; j++) {
      const Position *boundary_start = &boundaries[i].positions[j];
      const Position *boundary_end =
          &boundaries[i].positions[(j + 1) % boundaries[i].num_positions];

      for (int k = 0; k < 4; k++) {
        //check if the guard intersects with the boundaries
        if (is_intersect(current_pos, &next_corners[k], boundary_start,
             boundary_end)) {
            // Intersection detected with boundary
            // Change direction to opposite
            switch (*direction) {
          case UP:
              *direction = DOWN;
              break;
          case DOWN:
              *direction = UP;
              break;
          case LEFT:
              *direction = RIGHT;
              break;
          case RIGHT:
              *direction = LEFT;
              break;
          default:
              break;
            }
            
            // Adjust redraw region to the intersection point
            redraw_x = current_pos->x;
            redraw_y = current_pos->y;

            long long prev_pixels = get_rendered_pixels();

            // Update only the necessary portions of the background and sprite
            draw_rect_from_bitmap(erase_x, erase_y, update_width, update_height,
              background_cache_buffer);
            uart_puts("\nProcessed pixels: ");
            print_rendered_pixels();
            uart_puts(" | ");

            print_pixel_diff(prev_pixels, "[ERASED GUARD]");

            prev_pixels = get_rendered_pixels();

            copy_rect(redraw_x, redraw_y, 0, 0, SCREEN_WIDTH, update_width,
              update_height, game_map_bitmap, background_cache_buffer);
            draw_transparent_image(redraw_x, redraw_y, update_width,
              update_height, player_sprite_buffer);

            uart_puts("\nProcessed pixels: ");
            print_rendered_pixels();
            uart_puts(" | ");

            print_pixel_diff(prev_pixels, "[DRAWN GUARD]");

            return ; // Intersection detected
        }
      }
    }
  }

  // Adjust regions if the player moved partially off-screen
  if (erase_x < 0) {
    erase_x = 0;
    update_width += current_pos->x;
  }

  if (erase_y < 0) {
    erase_y = 0;
    update_height += current_pos->y;
  }

  if (redraw_x + PLAYER_WIDTH > SCREEN_WIDTH) {
    update_width = SCREEN_WIDTH - redraw_x;
  }

  if (redraw_y + PLAYER_HEIGHT > SCREEN_HEIGHT) {
    update_height = SCREEN_HEIGHT - redraw_y;
  }

  long long prev_pixels = get_rendered_pixels();

  // Update only the necessary portions of the background and sprite
  draw_rect_from_bitmap(erase_x, erase_y, update_width, update_height,
                        background_cache_buffer);
  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");


    print_pixel_diff(prev_pixels, "[ERASED GUARD]");

  prev_pixels = get_rendered_pixels();

  copy_rect(redraw_x, redraw_y, 0, 0, SCREEN_WIDTH, update_width, update_height,
            game_map_bitmap, background_cache_buffer);
  draw_transparent_image(redraw_x, redraw_y, update_width, update_height,
                         player_sprite_buffer);

  uart_puts("\nProcessed pixels: ");
  print_rendered_pixels();
  uart_puts(" | ");

    print_pixel_diff(prev_pixels, "[DRAWN GUARD]");

  // Update player's current position
  current_pos->x = next_pos.x;
  current_pos->y = next_pos.y;
}