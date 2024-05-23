//gengine-entity.h
#include "gengine-core.h"

typedef struct {
  Position position;
  Size size;
  Color color;
} Entity;

Entity create_entity(int x, int y, int width, int height, int r, int g, int b);
void move_entity(Entity *entity, int x, int y);
void resize_entity(Entity *entity, int width, int height);
void change_color(Entity *entity, int r, int g, int b);
void print_entity(Entity *entity);
