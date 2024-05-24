#include "../../headers/constants.h"
#include "../../headers/gengine-core.h"
#include "maps.h"

enum { OBJ_NONE = 0, OBJ_PLAYER = 1 };

struct Object {
  unsigned int type;
  unsigned int width;
  unsigned int height;
  unsigned char alive;
  Position position;
};

void start_unrob_game();
void move_player(char key);
void rotate_inventory(char key);
void draw_player();
void draw_items();
void draw_inventory(int selected_item);
void toggle_collision_debugger();
int get_collision_debugger_status();

void countdown(void);
void draw_time(void);
void draw_score(void);
