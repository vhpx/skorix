#include "../../headers/constants.h"
#include "../../headers/gengine-core.h"
#include "maps.h"

enum { OBJ_NONE = 0, OBJ_PLAYER = 1 };
enum { EMPTY_BOX = 0, INCORRECT_BOX = 1, CORRECT_BOX = 2, IN_RANGE_BOX = 3 };

static int collision_debugger = 0;

struct Object {
  unsigned int type;
  unsigned int width;
  unsigned int height;
  unsigned char alive;
  Position position;
};

void start_unrob_game();
void move_player(char key);
// void move_guard(struct Object *guard, int x, int y, int max_x, int max_y, int min_x, int min_y, int direction, int step_size, int *guard_direction);
void rotate_inventory(char key);
void draw_player();
void draw_items();
void draw_placement_boxes(Item *items, int num_items, int status);
void draw_inventory(int selected_item);
void update_placement_boxes(Position player_position, Item *items,
                            int num_items);
void move_items_to_final_position();
int get_collision_debugger_status();
void toggle_collision_debugger();

void countdown(void);
void draw_time(void);
void draw_score(void);
