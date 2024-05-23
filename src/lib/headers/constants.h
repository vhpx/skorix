#define true 1
#define false 0

#define MAX_CMD_ALIAS 10
#define MAX_CMD_USAGE 10
#define MAX_CMD_ARGS 10
#define MAX_CMD_SIZE 1000
#define MAX_STR_LENGTH 1000
#define MAX_ROW_TITLE_LENGTH 14
#define MAX_ROW_VALUE_LENGTH 39

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

#define MAX_GENGINE_ENTITIES 1000
#define MAX_GENGINE_MAPS 100
#define MAX_GENGINE_MAP_ROWS 100
#define MAX_GENGINE_MAP_COLUMNS 100

#define PLAYER_WIDTH 40
#define PLAYER_HEIGHT 40
#define GENGINE_STEP_SIZE 20

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
