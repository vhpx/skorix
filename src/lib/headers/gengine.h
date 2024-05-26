// gengine.h
#include "constants.h"
#include "gengine-core.h"

#ifndef __GENGINE_H__
#define __GENGINE_H__

int is_intersect(const Position *p1, const Position *q1, const Position *p2,
                 const Position *q2);
void render_boundary(Position *boundaries, int num_boundaries);
void render_boundaries(const Boundary *boundaries, int num_boundaries);
void move_in_boundaries(const Boundary *boundaries, int num_boundaries,
                        enum Direction direction, Position *current_pos,
                        const unsigned long *game_map_bitmap,
                        unsigned long *background_cache_buffer,
                        unsigned long *player_sprite_buffer, int force_redraw,
                        int is_guard);
void move_guard_in_boundaries(const Boundary *boundaries, int num_boundaries,
                              enum Direction *direction, Position *current_pos,
                              const Bitmap *game_map_bitmap,
                              Bitmap *background_cache_buffer,
                              const Bitmap *player_sprite_buffer,
                              int force_redraw);

#endif
