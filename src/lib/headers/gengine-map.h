// gengine-map.h
#include "constants.h"

#ifndef __GENGINE_MAP_H__
#define __GENGINE_MAP_H__

typedef struct {
  int rows;
  int columns;
  int map[SCREEN_WIDTH][SCREEN_WIDTH];
} GameMap;

void display_game_map(GameMap *map);

#endif
