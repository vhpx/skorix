//gengine.h
#include "constants.h"
#include "gengine-entity.h"
#include "gengine-map.h"

#ifndef __GENGINE_H__
#define __GENGINE_H__

struct Game {
  Entity entities[MAX_GENGINE_ENTITIES];
  GameMap maps[MAX_GENGINE_MAPS];
  int num_entities;
  int num_maps;
};

void game_loop();

#endif