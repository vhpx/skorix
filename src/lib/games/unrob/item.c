#include "../../headers/constants.h"
#include "../../headers/gengine-core.h"

typedef struct {
  int id;
  char name[MAX_GENGINE_ITEM_NAME_LENGTH];
  Position position;
  Size size;
} Item;