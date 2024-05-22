#include "../../headers/constants.h"

// The screen
#define MARGIN 0
#define VIRTWIDTH (WIDTH - (2 * MARGIN))
#define ITEM_SIZE 40 // square

#define MOVE_STEP 20
#define MAP_SCALE_DIVIDER_WIDTH (SCREEN_WIDTH / MOVE_STEP)
#define MAP_SCALE_DIVIDER_HEIGHT (SCREEN_HEIGHT / MOVE_STEP)

#define GAME_TIME_ZOOM 3

void start_unrob_game();
void movePlayer(char key);
void rotate_inventory(char key);
void display_inventory(int selected_item);

void countdown(void);
void draw_time(void);
