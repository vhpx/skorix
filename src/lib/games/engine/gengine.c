//gengine.c
#include "../../headers/gengine.h"
#include "../../headers/print.h"
#include "../../headers/uart0.h"

// implement game loop
void game_loop() {
  // Initialize the console
  setup_console();
  clear_screen();
  print_title("Game Engine");

  // Create an entity
  Entity entity = create_entity(10, 10, 20, 20, 255, 0, 0);
  print_entity(&entity);

  // Move the entity
  move_entity(&entity, 20, 20);
  print_entity(&entity);

  // Resize the entity
  resize_entity(&entity, 30, 30);
  print_entity(&entity);

  // Change the color of the entity
  change_color(&entity, 0, 255, 0);
  print_entity(&entity);

  // Create a game map
  // GameMap map = {
  //     .rows = 10,
  //     .columns = 10,
  //     .map =
  //         {
  //             {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  //             {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  //             {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
  //             {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
  //             {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
  //             {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  //             {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
  //             {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  //             {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
  //             {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  //         },
  // };

  // display_game_map(&map);

}
