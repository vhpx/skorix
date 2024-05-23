// entity.c
#include "../../headers/gengine-entity.h"
#include "../../headers/string.h"
#include "../../headers/uart0.h"

Entity create_entity(int x, int y, int width, int height) {
  Entity entity;
  entity.position.x = x;
  entity.position.y = y;
  entity.size.width = width;
  entity.size.height = height;

  return entity;
}

void move_entity(Entity *entity, int x, int y) {
  entity->position.x = x;
  entity->position.y = y;
}

void resize_entity(Entity *entity, int width, int height) {
  entity->size.width = width;
  entity->size.height = height;
}

void print_entity(Entity *entity) {
  // Declare variables to store the integer values as strings
  char x[10], y[10], width[10], height[10], r[3], g[3], b[3];

  // Convert the integer values to strings
  int2str(entity->position.x, x);
  int2str(entity->position.y, y);
  int2str(entity->size.width, width);
  int2str(entity->size.height, height);

  // Print the entity details
  uart_puts("Entity: x=");
  uart_puts(x);
  uart_puts(", y=");
  uart_puts(y);
  uart_puts(", width=");
  uart_puts(width);
  uart_puts(", height=");
  uart_puts(height);
  uart_puts("\n");
}
