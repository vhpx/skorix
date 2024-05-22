//map.c
#include "../../headers/gengine-map.h"
#include "../../headers/uart0.h"

void display_game_map(GameMap *map) {
  for (int i = 0; i < map->rows; i++) {
    for (int j = 0; j < map->columns; j++) {
      uart_sendc(map->map[i][j] + '0');
    }
    uart_sendc('\n');
  }
}
