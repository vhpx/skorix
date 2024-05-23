// gengine.h
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

void render_boundary(Position *boundaries, int num_boundaries);
void move_in_boundaries(Boundary *boundaries, int num_boundaries, char key,
                        Position *current_pos, unsigned long *game_map_bitmap,
                        unsigned long *background_cache_buffer,
                        unsigned long *player_sprite_buffer);

#endif
