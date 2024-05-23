// gengine.h
#include "constants.h"
#include "gengine-core.h"

#ifndef __GENGINE_H__
#define __GENGINE_H__

void render_boundary(Position *boundaries, int num_boundaries);
void move_in_boundaries(Boundary *boundaries, int num_boundaries, char key,
                        Position *current_pos,
                        const unsigned long *game_map_bitmap,
                        unsigned long *background_cache_buffer,
                        unsigned long *player_sprite_buffer);

#endif
