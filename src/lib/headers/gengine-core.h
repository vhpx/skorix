// gengine-core.h

#include "constants.h"

#ifndef __GENGINE_CORE_H__
#define __GENGINE_CORE_H__

typedef struct {
  int x;
  int y;
} Position;

typedef struct {
  const Position *positions;
  const int num_positions;
} Boundary;

typedef struct {
  const int width;
  const int height;
} Size;

typedef struct {
  Position position;
  const Size size;
  const unsigned long *bitmap;
} Entity;

typedef struct {
  const int id;
  const char name[MAX_GENGINE_ITEM_NAME_LENGTH];
  Entity entity;
} Item;

typedef struct {
  const Position spawn_point;
  const unsigned long *bitmap;
  const Size size;
  const Item *items;
  const Boundary *boundaries;
  const int num_boundaries;
} GameMap;

struct Game {
  Entity entities[MAX_GENGINE_ENTITIES];
  GameMap *maps;
  int num_entities;
  const int num_maps;
};

#endif
