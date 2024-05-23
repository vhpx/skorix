// gengine-entity.h
#include "gengine-core.h"

typedef struct {
  Position position;
  Size size;
} Entity;

Entity create_entity(int x, int y, int width, int height);
void move_entity(Entity *entity, int x, int y);
void resize_entity(Entity *entity, int width, int height);
void print_entity(Entity *entity);
