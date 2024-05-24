// gengine-core.h

#include "constants.h"

#ifndef __GENGINE_CORE_H__
#define __GENGINE_CORE_H__

typedef unsigned long Bitmap;

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
  const Bitmap *bitmap;
} Entity;

typedef struct {
  const int id;
  char *name;
  Position final_position;
  Position placement_position;
  Entity entity;
} Item;

typedef struct {
  const Position spawn_point;
  const Bitmap *bitmap;
  const Size size;
  Item *items;
  const Boundary *boundaries;
  const int num_items;
  const int num_boundaries;
} GameMap;

struct Game {
  Entity *entities;
  GameMap *maps;
  int num_entities;
  const int num_maps;
};

#endif
