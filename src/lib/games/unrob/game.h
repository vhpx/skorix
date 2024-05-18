// The screen
#define WIDTH 1000
#define HEIGHT 1000
#define MARGIN 0
#define VIRTWIDTH (WIDTH - (2 * MARGIN))
#define FONT_BPG 8

#define SCREEN_GAME_WIDTH 1000
#define SCREEN_GAME_HEIGHT 1000
#define MAP_WIDTH 1000
#define MAP_HEIGHT 1000
#define MOVE_STEP 20
#define MAP_SCALE_DIVIDER_WIDTH (MAP_WIDTH/MOVE_STEP)
#define MAP_SCALE_DIVIDER_HEIGHT (MAP_HEIGHT/MOVE_STEP)

void start_unrob_game();
void movePlayer(char key);