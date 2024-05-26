#define true 1
#define false 0

#define MAX_CMD_ALIAS 10
#define MAX_CMD_USAGE 10
#define MAX_CMD_ARGS 10
#define MAX_CMD_SIZE 1000
#define MAX_STR_LENGTH 1000
#define MAX_ROW_TITLE_LENGTH 14
#define MAX_ROW_VALUE_LENGTH 39

#define MARGIN 0
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

#define MAX_GENGINE_ITEMS 10
#define MAX_GENGINE_ITEM_NAME_LENGTH 20
#define MAX_GENGINE_MAPS 3
#define MAX_GENGINE_BOUNDARIES 10
#define MAX_GENGINE_ENTITIES 1000

#define GENGINE_PLACEMENT_RANGE 100
#define GENGINE_ITEM_SIZE 40
#define GENGINE_STEP_SIZE 14
#define GENGINE_TIME_ZOOM 3

#define PLAYER_WIDTH 42
#define PLAYER_HEIGHT 60

#define FLOAT_MAX 1000000.0

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

typedef struct {
  char *name;
  char *email;
  char *website;
  char *github;
  char *linkedin;
} Creator;

typedef struct {
  char *id;
  char *name;
  char *version;
  char *release_date;
  char *prefix;
  char *logo_mark;
  Creator creators[4];
} OSInfo;

extern OSInfo OS_INFO;

#endif
