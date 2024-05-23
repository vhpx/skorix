// gengine-core.h

#include "constants.h"

#ifndef __GENGINE_CORE_H__
#define __GENGINE_CORE_H__

typedef struct {
  int x;
  int y;
} Position;

typedef struct {
  Position *positions;
  int num_positions;
} Boundary;

typedef struct {
  int width;
  int height;
} Size;

typedef struct {
  Position position;
  Size size;
} Entity;

typedef struct {
  int id;
  char name[MAX_GENGINE_ITEM_NAME_LENGTH];
  Position position;
  Size size;
} Item;

typedef struct {
  int rows;
  int columns;
  int map[SCREEN_WIDTH][SCREEN_WIDTH];
} GameMap;

struct Game {
  Entity entities[MAX_GENGINE_ENTITIES];
  GameMap maps[MAX_GENGINE_MAPS];
  int num_entities;
  int num_maps;
};

#endif
