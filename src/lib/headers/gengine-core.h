// gengine-core.h

#include "constants.h"
#include "framebf.h"

#ifndef __GENGINE_CORE_H__
#define __GENGINE_CORE_H__

enum Direction { UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3 };

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
  Entity entity;
  const int step;
  enum Direction direction;
  const Boundary *boundaries;
} Guard;

typedef struct {
  const Position spawn_point;
  const Boundary *boundaries;
  const Bitmap *bitmap;
  const Size size;
  Item *items;
  Guard *guards;
  const int num_boundaries;
  const int num_items;
  const int num_guards;
} GameMap;

struct Game {
  Entity *entities;
  GameMap *maps;
  int num_entities;
  const int num_maps;
};

#endif
