#include "config.h"
#include "constants.h"

#ifndef __COMMAND_H__
#define __COMMAND_H__

#define MAX_PAST_CMD 21

typedef struct {
  char *name;
  char *description;
  void (*func)();
  char *aliases[MAX_CMD_ALIAS];
  char *usage[MAX_CMD_USAGE];
} Command;

typedef struct {
  char tag[MAX_CMD_SIZE];
  char value[MAX_CMD_SIZE];
} Tag;

typedef struct {
  char commands[MAX_PAST_CMD][MAX_CMD_SIZE];
  int size;
} CommandHistory;

extern Command commands[MAX_CMD_SIZE];

void empty_func();
int execute_command(char *input, CommandHistory *cmd_history);
void save_command(char *command, CommandHistory *cmd_history,
                  int *past_cmd_index);
void autofill_command(char *buffer, char *completed_command,
                      char *pre_autofilled_cmd, char *post_autofilled_cmd);
void restart_uart();
void set_color(Tag tags[MAX_CMD_ARGS]);
void set_config(Tag tags[MAX_CMD_ARGS]);
void play_game(Tag tags[MAX_CMD_ARGS]);
void handle_valid_key(char c);
void handle_invalid_key(char c);

#endif
