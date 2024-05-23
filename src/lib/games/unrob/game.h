#include "../../headers/constants.h"

// The screen
#define MARGIN 0
#define VIRTWIDTH (WIDTH - (2 * MARGIN))
#define ITEM_SIZE 40 // square

#define STEP_SIZE 10
#define MAP_SCALE_DIVIDER_WIDTH (SCREEN_WIDTH / STEP_SIZE)
#define MAP_SCALE_DIVIDER_HEIGHT (SCREEN_HEIGHT / STEP_SIZE)

#define GAME_TIME_ZOOM 3

void start_unrob_game();
void move_player(char key);
void rotate_inventory(char key);
void draw_player();
void draw_inventory(int selected_item);
void toggle_collision_debugger();
int get_collision_debugger_status();

void countdown(void);
void draw_time(void);
