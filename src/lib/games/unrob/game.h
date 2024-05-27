#include "../../headers/constants.h"
#include "../../headers/gengine-core.h"
#include "maps.h"

enum Box {
  EMPTY_BOX = 0,
  INCORRECT_BOX = 1,
  CORRECT_BOX = 2,
  IN_RANGE_BOX = 3
};

enum Action { NO_ACTION = 0, PLACE_DOWN = 1, PICK_UP = 2, SWAP_ITEM = 3 };

void start_unrob_game();
void move_player(char key);
void switch_inventory_item(char key);
const Bitmap *get_player_sprite();
const Bitmap *get_guard_sprite(enum Direction direction);
void draw_player();
void draw_final_items();
void draw_placed_items();
void draw_placement_boxes(Item *items, int num_items, enum Box box);
void display_selected_item(int selected_item, Item *items, int num_items);
void update_placement_boxes(Position player_position, Item *items,
                            int num_items);
void move_items_to_final_position_original();
void move_items_to_final_position(Item *items, int num_items, const Bitmap *bitmap);
void toggle_rendering_debugger();
void toggle_game_debugger();
int is_intersect_guard(const Position *a, const Position *b, const Position *c,
                       const Position *d);

void countdown(void);
void draw_time(void);
void draw_score(void);
void game_over(void);
void draw_item_with_box(Item *item, enum Box box);
void execute_main_action(void);
void select_map(int map_num);
void level_selector(void);
void draw_level_selection_base(int selected_level);
void select_level(char key);
void game_start_selector(void);
void select_game_start_exit(char key);
void start_timer3(void);
void shuffleItems(Item *items, int num_items, int n);


extern int is_game_over;
extern int is_game_start;
extern int select_game_option;
extern int is_level_selected;
extern int selected_level;
extern int enable_game_debugger;
extern int enable_rendering_debugger;