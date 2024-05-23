#include "../headers/print.h"
#include "../headers/color.h"
#include "../headers/command.h"
#include "../headers/constants.h"
#include "../headers/mbox.h"
#include "../headers/number.h"
#include "../headers/string.h"
#include "../headers/uart0.h"

#define cmd_trailing_tag uart_puts("> ")

void print_divider(int nline) {
  uart_puts("+");
  for (int i = 0; i < MAX_ROW_TITLE_LENGTH + MAX_ROW_VALUE_LENGTH + 5; i++) {
    uart_puts("-");
  }
  uart_puts("+");
  if (nline)
    new_line_with_color();
}

void new_line_with_color() {
  uart_puts("\n");
  format_and_print_string("", OS_CONFIG.text_color, OS_CONFIG.background_color);
}

void new_line_force_color(const char *color_code, const char *bg_color_code) {
  uart_puts("\n");
  format_and_print_string("", color_code, bg_color_code);
}

void print_cmd_not_found() {
  new_line_with_color();
  format_and_print_string("Error: Command not found", OS_CONFIG.text_color,
                          OS_CONFIG.background_color);
  new_line_with_color();
  format_and_print_string("Type 'help' to see available commands.",
                          OS_CONFIG.text_color, OS_CONFIG.background_color);
  new_line_with_color();
  new_line_with_color();
}

void print_cmd_not_implemented() {
  new_line_with_color();
  uart_puts("Error: This command is not implemented yet.\n\n");
}

void print_row(char *name, char *value, int link) {
  uart_puts("| ");

  uart_puts(name);

  // Calculate the number of spaces to add
  int spaces = MAX_ROW_TITLE_LENGTH - strlen_no_color(name);

  // Add spaces
  for (int i = 0; i < spaces; i++) {
    uart_puts(" ");
  }

  uart_puts(" | ");

  // Check if the value string is too long
  if (strlen_no_color(value) > MAX_ROW_VALUE_LENGTH) {
    // Truncate the value string and append "..."
    char truncated_value[MAX_ROW_VALUE_LENGTH];
    strncpy(truncated_value, value, MAX_ROW_VALUE_LENGTH - 3);
    truncated_value[MAX_ROW_VALUE_LENGTH - 3] = '\0';
    append_str(truncated_value, "...");
    uart_puts(truncated_value);
    spaces = 0;
  } else {
    uart_puts(value);
    // Calculate the number of spaces to add
    spaces = MAX_ROW_VALUE_LENGTH - strlen_no_color(value);
  }

  // Add spaces
  for (int i = 0; i < spaces; i++) {
    uart_puts(" ");
  }

  uart_puts(" |\n");
}

void print_title_centered(char *title) {
  new_line_with_color();
  print_divider(1);
  uart_puts("| ");

  // Calculate the total number of spaces
  int total_spaces =
      MAX_ROW_TITLE_LENGTH + MAX_ROW_VALUE_LENGTH - strlen_no_color(title) + 3;

  // Calculate the number of spaces to add before and after the title
  int spaces_before = total_spaces / 2;
  int spaces_after = total_spaces - spaces_before;

  // Add spaces before the title
  for (int i = 0; i < spaces_before; i++) {
    uart_puts(" ");
  }

  uart_puts(title);

  // Add spaces after the title
  for (int i = 0; i < spaces_after; i++) {
    uart_puts(" ");
  }

  uart_puts(" |\n");
  print_divider(1);
}

void print_title(char *title) {
  print_divider(1);
  uart_puts("| ");
  uart_puts(title);

  // Calculate the number of spaces to add
  int spaces =
      MAX_ROW_TITLE_LENGTH + MAX_ROW_VALUE_LENGTH - strlen_no_color(title) + 3;

  // Add spaces
  for (int i = 0; i < spaces; i++) {
    uart_puts(" ");
  }

  uart_puts(" |\n");
  print_divider(1);
}

void print_os_info() {
  print_row("Name", OS_INFO.name, 0);
  print_row("Version", OS_INFO.version, 0);
  print_row("Release Date", OS_INFO.release_date, 0);
}

void print_creator_info() {
  for (int i = 0; i < sizeof(OS_INFO.creators) / sizeof(Creator); i++) {
    if (!OS_INFO.creators[i].name || *OS_INFO.creators[i].name == '\0' ||
        strlen(OS_INFO.creators[i].name) == 0) {
      break;
    }

    char creator[MAX_STR_LENGTH];
    char index[3];

    clrstr(creator);
    clrstr(index);
    int2str(i + 1, index);

    append_str(creator, "Contributer ");
    append_str(creator, index);

    print_row(creator, OS_INFO.creators[i].name, 0);
    print_row("Email", OS_INFO.creators[i].email, 0);
    print_divider(1);
  }
}

void print_system_info() {
  uart_puts(OS_INFO.logo_mark);

  new_line_with_color();
  new_line_with_color();

  print_divider(1);
  print_os_info();
  print_divider(1);
  print_creator_info();
  new_line_with_color();
}

void print_welcome_message() {
  uart_puts(
      "d88888b d88888b d88888b d888888b .d888b.   j88D  .d888b.  .d88b.  \n"
      "88'     88'     88'     `~~88~~' VP  `8D  j8~88  88' `8D .8P  88. \n"
      "88ooooo 88ooooo 88ooooo    88       odD' j8' 88  `V8o88' 88  d'88 \n"
      "88~~~~~ 88~~~~~ 88~~~~~    88     .88'   V88888D    d8'  88 d' 88 \n"
      "88.     88.     88.        88    j88.        88    d8'   `88  d8' \n"
      "Y88888P Y88888P Y88888P    YP    888888D     VP   d8'     `Y88P'  \n"
      "                                                                  \n"
      "                                                                  \n"
      "d8888b.  .d8b.  d8888b. d88888b    .d88b.  .d8888.                \n"
      "88  `8D d8' `8b 88  `8D 88'       .8P  Y8. 88'  YP                \n"
      "88oooY' 88ooo88 88oobY' 88ooooo   88    88 `8bo.                  \n"
      "88~~~b. 88~~~88 88`8b   88~~~~~   88    88   `Y8b.                \n"
      "88   8D 88   88 88 `88. 88.       `8b  d8' db   8D                \n"
      "Y8888P' YP   YP 88   YD Y88888P    `Y88P'  `8888Y'"
      "\n\n");

  print_divider(1);
  print_system_info();

  uart_puts("Welcome to Skorix, a simple OS for the Raspberry Pi 3 & 4.\n");
  uart_puts("Type 'help' to see available commands.\n\n");
}

void print_cmd_history(CommandHistory *cmd_history) {
  uart_puts("\n");

  uart_puts("+");
  for (int i = 0; i < MAX_ROW_TITLE_LENGTH + MAX_ROW_VALUE_LENGTH + 5; i++) {
    uart_puts("-");
  }
  uart_puts("+");
  new_line_with_color();

  uart_puts("| ");

  uart_puts("Index");

  // Calculate the number of spaces to add
  int spaces = MAX_ROW_TITLE_LENGTH - 5;

  // Add spaces
  for (int i = 0; i < spaces; i++) {
    uart_puts(" ");
  }

  uart_puts(" | ");

  uart_puts("Commands");

  // Calculate the number of spaces to add
  spaces = MAX_ROW_VALUE_LENGTH - 8;

  // Add spaces
  for (int i = 0; i < spaces; i++) {
    uart_puts(" ");
  }

  uart_puts(" |\n");

  uart_puts("+");
  for (int i = 0; i < MAX_ROW_TITLE_LENGTH + MAX_ROW_VALUE_LENGTH + 5; i++) {
    uart_puts("-");
  }
  uart_puts("+");
  new_line_with_color();

  for (int i = cmd_history->size; i >= 0; i--) {
    char index[3];
    int2str(cmd_history->size - i, index);
    print_row(index, cmd_history->commands[i], 0);
  }

  print_divider(1);
  new_line_with_color();
}

void print_echo(char *message) {
  uart_puts("\n");
  uart_puts(message);
  new_line_with_color();
}

void print_help(char *command) {
  uart_puts("\n");

  // If command is an empty string, print information about all commands
  if (strcmp(command, "") == 0 && strlen(command) == 0) {
    for (int i = 0; i < sizeof(commands) / sizeof(Command); i++) {
      // Check if command is not null
      if (!commands[i].name || !commands[i].func || *commands[i].name == '\0' ||
          strlen(commands[i].name) == 0) {
        break;
      }

      print_cmd_info(&commands[i], 1);
    }
  } else {
    // Try to find a command with the specified name
    for (int i = 0; i < sizeof(commands) / sizeof(Command); i++) {
      // Check the name
      if (strcmp(command, commands[i].name) == 0) {
        print_cmd_info(&commands[i], 0);
        return;
      } else {
        // Check aliases
        char **alias = commands[i].aliases;
        while (*alias) {
          if (strcmp(command, *alias) == 0) {
            print_cmd_info(&commands[i], 0);
            return;
          }
          alias++;
        }
      }
    }

    // If no such command is found, print an error message
    new_line_with_color();
    print_cmd_not_found();
  }
}

void print_cmd_info(Command *command, int indent) {
  char title[MAX_STR_LENGTH];

  // Clear the title
  for (int i = 0; i < MAX_STR_LENGTH; i++) {
    title[i] = '\0';
  }

  append_str(title, "@> ");
  append_str(title, command->name);
  print_title(title);

  uart_puts("| ");

  uart_puts("description: ");

  char description[MAX_STR_LENGTH];

  // Clear the description
  for (int i = 0; i < MAX_STR_LENGTH; i++) {
    description[i] = '\0';
  }

  append_str(description, command->description);

  str_auto_newline(description, MAX_ROW_VALUE_LENGTH + MAX_ROW_TITLE_LENGTH - 9,
                   "| ", " |",
                   MAX_ROW_VALUE_LENGTH + MAX_ROW_TITLE_LENGTH - 10);
  uart_puts(description);
  new_line_with_color();

  if (command->aliases[0] && *command->aliases[0] != '\0') {

    char aliases[MAX_STR_LENGTH];

    // Clear the aliases
    for (int i = 0; i < MAX_STR_LENGTH; i++) {
      aliases[i] = '\0';
    }

    append_str(aliases, "| ");
    append_str(aliases, "aliases: ");

    char **alias = command->aliases;
    while (*alias) {
      append_str(aliases, *alias);
      if (*(alias + 1)) {
        append_str(aliases, ", ");
      }
      alias++;
    }
    uart_puts(aliases);

    // Calculate the number of spaces to add
    int spaces = MAX_ROW_TITLE_LENGTH + MAX_ROW_VALUE_LENGTH -
                 strlen_no_color(aliases) + 5;

    // Add spaces
    for (int i = 0; i < spaces; i++) {
      uart_puts(" ");
    }

    uart_puts(" |\n");
  }

  if (indent == 0 && command->usage[0] && *command->usage[0] != '\0') {

    char usages[MAX_STR_LENGTH];

    // Clear the usages
    for (int i = 0; i < MAX_STR_LENGTH; i++) {
      usages[i] = '\0';
    }

    append_str(usages, "| ");
    append_str(usages, "usage: ");

    char **usage = command->usage;
    while (*usage) {
      append_str(usages, *usage);
      if (*(usage + 1)) {
        append_str(usages, ", ");
      }
      usage++;
    }
    uart_puts(usages);

    // Calculate the number of spaces to add
    int spaces = MAX_ROW_TITLE_LENGTH + MAX_ROW_VALUE_LENGTH -
                 strlen_no_color(usages) + 5;

    // Add spaces
    for (int i = 0; i < spaces; i++) {
      uart_puts(" ");
    }

    uart_puts(" |");
    new_line_with_color();
  }

  print_divider(0);
  uart_puts("\n\n");
}

void clear_screen() {
  // Move the cursor to the top left
  uart_puts("\033[H");
  // Clear the screen
  uart_puts("\033[J");
}

void setup_console() {
  // set up serial console
  uart_init(0);
}

void reset_console() {
  uart_puts("\033[2K\r");
  format_and_print_string(OS_INFO.prefix, OS_CONFIG.text_color,
                          OS_CONFIG.background_color);
  cmd_trailing_tag;
}

void query_and_print(char *name, unsigned int tag,
                     void (*parse_func)(unsigned int, unsigned int, char *)) {
  mBuf[0] = 8 * 4;        // Buffer size in bytes
  mBuf[1] = MBOX_REQUEST; // Message Request Code (this is a request message)
  mBuf[2] = tag;          // TAG Identifier
  mBuf[3] = 8;
  mBuf[4] = 0;
  mBuf[5] = 0;
  mBuf[6] = 0; // clear output buffer (response data is mBuf[5] & mBuf[6])
  mBuf[7] = MBOX_TAG_LAST;
  char result[18];

  if (mbox_call(ADDR(mBuf), MBOX_CH_PROP)) {
    parse_func(mBuf[5], mBuf[6], result);
    print_row(name, result, 0);
  } else {

    uart_puts("Failed to query ");
    uart_puts(name);
    uart_puts(".\n");
  }
}

void print_board_info() {
  print_divider(1);

  query_and_print("MAC Address", 0x00010003, parse_mac_address);
  query_and_print("Board Revision", 0x00010002, parse_board_revision);

  print_divider(1);

  char baud_rate_str[10], dbit_str[5], sbit_str[5];

  int2str(BAUD_RATE_CONFIG_VALUE, baud_rate_str);
  int2str(DBIT_CONFIG, dbit_str);
  int2str(SBIT_CONFIG, sbit_str);

  append_str(baud_rate_str, " bps");
  append_str(dbit_str, " bits");
  append_str(sbit_str, SBIT_CONFIG == 1 ? " bit" : " bits");

  print_row("Baud Rate", baud_rate_str, 0);
  print_row("Data Bits", dbit_str, 0);
  print_row("Stop Bits", sbit_str, 0);
  print_row("Parity Mode",
            PARITY_MODE_CONFIG == 0   ? "None"
            : PARITY_MODE_CONFIG == 1 ? "Odd"
            : PARITY_MODE_CONFIG == 2 ? "Even"
                                      : "Unknown",
            0);
  print_row("Flow Control",
            SEND_FLOW_CONFIG == 0   ? "Default (No Flow Control)"
            : SEND_FLOW_CONFIG == 1 ? "CTS/RTS mode"
                                    : "Unknown",
            0);

  print_divider(1);
  new_line_with_color();
}

void format_and_print_string(const char *str, const char *color_code,
                             const char *bg_color_code) {
  if (!str || !color_code || !bg_color_code) {
    return; // Invalid input
  }

  static char formatted_str[MAX_CMD_SIZE];
  formatted_str[0] = '\0';
  int i = 0, j = 0;

  // Append color_code
  while (color_code[i] != '\0' && i < MAX_CMD_SIZE - 1) {
    formatted_str[j++] = color_code[i++];
  }

  i = 0;
  // Append bg_color_code
  while (bg_color_code[i] != '\0' && j < MAX_CMD_SIZE - 1) {
    formatted_str[j++] = bg_color_code[i++];
  }

  i = 0;
  // Append str
  while (str[i] != '\0' && j < MAX_CMD_SIZE - 1) {
    formatted_str[j++] = str[i++];
  }

  // Null terminate the string
  formatted_str[j] = '\0';

  uart_puts(formatted_str);
}
