#include "command.h"
#include "config.h"

int cli();
int run_cli();
void clear_screen();
void show_info();
int handle_input(char c, char *cli_buffer, int *index, int *past_cmd_index,
                 CommandHistory *cmd_history, char *pre_autofilled_cmd,
                 char *post_autofilled_cmd);
void print_command_info(Command *command, int indent);
void help(char *command);
void handle_backspace(char *cli_buffer, int *index, char *pre_autofilled_cmd,
                      char *post_autofilled_cmd);
void handle_autocomplete(char *cli_buffer, int *index, char *pre_autofilled_cmd,
                         char *post_autofilled_cmd);
void handle_history_navigation(char c, char *cli_buffer, int *index,
                               int *past_cmd_index,
                               CommandHistory *cmd_history);
void handle_regular_input(char c, char *cli_buffer, int *index,
                          char *pre_autofilled_cmd, char *post_autofilled_cmd);
int handle_newline(char *cli_buffer, int *index, int *past_cmd_index,
                   CommandHistory *cmd_history);