#include "command.h"

void print_divider(int nline);
void print_row(char *name, char *value, int link);
void print_title(char *title);
void print_title_centered(char *title);
void print_system_info();
void print_os_info();
void print_creator_info();
void print_welcome_message();
void print_cmd_not_found();
void print_cmd_not_implemented();
void print_echo(char *message);
void print_help(char *command);
void print_cmd_info(Command *command, int indent);
void print_cmd_history(CommandHistory *cmd_history);
void print_board_info();
void clear_screen();
void setup_console();
void reset_console();
void query_and_print(char *name, unsigned int tag,
                     void (*parse_func)(unsigned int, unsigned int, char *));
void format_and_print_string(const char *str, const char *color_code,
                             const char *bg_color_code);
void new_line_with_color();
void new_line_force_color(const char *color_code, const char *bg_color_code);
