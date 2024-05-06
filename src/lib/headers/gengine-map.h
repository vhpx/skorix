#include "constants.h"

#ifndef __GENGINE_MAP_H__
#define __GENGINE_MAP_H__

typedef struct {
  int rows;
  int columns;
  int map[MAX_GENGINE_MAP_ROWS][MAX_GENGINE_MAP_COLUMNS];
} GameMap;

void display_game_map(GameMap *map);

#endif