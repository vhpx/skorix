// This is command.c
#include "../headers/command.h"
#include "../../img/img.h"
#include "../../img/img_src.h"
#include "../../video/video.h"
#include "../games/unrob/game.h"
#include "../headers/breakout.h"
#include "../headers/cli.h"
#include "../headers/color.h"
#include "../headers/config.h"
#include "../headers/font.h"
#include "../headers/print.h"
#include "../headers/string.h"
#include "../headers/uart0.h"

Command commands[] = {
    {
        "help",                    // Name
        "Show available commands", // Description
        print_help,                // Func
        {"h"},                     // Aliases
    },
    {
        "help <command>",                            // Name
        "Show information about a specific command", // Description
        print_help,                                  // Func
        {"h <cmd>"},                                 // Aliases
    },
    {
        "clear",            // Name
        "Clear the screen", // Description
        clear_screen,       // Func
        {"cls", "clr"},     // Aliases
    },
    {
        "setcolor",                                         // Name
        "Set text and background color for console",        // Description
        set_color,                                          // Func
        {"color", "sc"},                                    // Aliases
        {"setcolor -t <text_color> -b <background_color>"}, // Usage
    },
    {
        "showinfo",                                  // Name
        "Show board revision and board MAC address", // Description
        print_board_info,                            // Func
        {"sinf", "binfo", "bi"},                     // Aliases
    },
    {
        "about",                             // Name
        "Show information about the system", // Description
        print_system_info,                   // Func
        {"info", "i", "ab"},                 // Aliases
    },
    {
        "history",                      // Name
        "Show the history of commands", // Description
        print_cmd_history,              // Func
        {"hist", "hst"},                // Aliases
    },
    {
        "reload",                   // Name
        "Reload the system (uart)", // Description
        restart_uart,               // Func
    },
    {
        "shutdown",            // Name
        "Shutdown the system", // Description
        empty_func,            // Func
    },
    {
        "echo",                                // Name
        "Prints the arguments to the console", // Description
        print_echo,                            // Func
        {"ec"},                                // Aliases
    },
    {
        "config",               // Name
        "Configure the system", // Description
        set_config,             // Func
        {"conf", "cfg", "cf"},  // Aliases
        {"config -b <baudrate> -d <databits> -p <parity> -s <stopbits> -f "
         "<flow_control>"}, // Usage
    },
    {
        "image",            // Name
        "Display an image", // Description
        display_image,      // Func
    },
    {
        "video",           // Name
        "Display a video", // Description
        display_video,     // Func
    },
    {
        "font",           // Name
        "Display a font", // Description
        displayFont,      // Func
    },
    {
        "play",        // Name
        "Play a game", // Description
        play_game,     // Func
        {"p"},         // Aliases
        {"play -g <game> | Available games:\n- unrob (ur)\n- breakout "
         "(bk)"} // Usage
    },
};

void empty_func() {}

char *get_cmd_name(char *command) {
  // Skip leading spaces
  while (*command && (*command == ' ')) {
    command++;
  }

  // Move to the end of the command name
  char *end = command;
  while (*end && (*end != ' ')) {
    end++;
  }

  // Add a null character at the end of the command name
  *end = '\0';

  // Return the command name
  return command;
}

char *get_cmd_arg_list(char *command) {
  // Skip leading spaces
  skip_spaces(&command);

  // Move to the end of the command name
  while (*command && (*command != ' ')) {
    command++;
  }

  // Skip spaces between command name and arguments
  skip_spaces(&command);

  // Return the command arguments
  return command;
}

void get_cmd_args(char *argList, Tag tags[MAX_CMD_ARGS]) {
  char *arg = argList;
  int tagIndex = -1;
  int isTag = 0;

  while (*arg) {
    if (*arg == '-') {
      tagIndex++;
      tags[tagIndex].tag[0] = '\0';
      tags[tagIndex].value[0] = '\0';
      isTag = 1;
    } else if (*arg == ' ') {
      isTag = 0;
    } else if (isTag) {
      strncat(tags[tagIndex].tag, arg, 1);
    } else {
      strncat(tags[tagIndex].value, arg, 1);
    }
    arg++;
  }
}

int execute_command(char *input, CommandHistory *cmd_history) {
  char *command_args = get_cmd_arg_list(input);
  char *command_name = get_cmd_name(input);

  Tag tags[MAX_CMD_ARGS];

  // Reset tags to zero
  for (int i = 0; i < MAX_CMD_ARGS; i++) {
    tags[i].tag[0] = '\0';
    tags[i].value[0] = '\0';
  }

  get_cmd_args(command_args, tags);

  // Trim the command name and arguments
  trim(command_name);
  trim(command_args);

  if (strcmp(command_name, "") == 0) {
    return 0;
  }

  if (strcmp(command_name, "shutdown") == 0) {
    return -1;
  }

  if (strcmp(command_name, "help") == 0 || strcmp(command_name, "h") == 0) {
    print_help(command_args);
    return 0;
  }

  if (strcmp(command_name, "history") == 0 ||
      strcmp(command_name, "hist") == 0 || strcmp(command_name, "hst") == 0) {
    print_cmd_history(cmd_history);
    return 0;
  }

  if (strcmp(command_name, "echo") == 0 || strcmp(command_name, "ec") == 0) {
    print_echo(command_args);
    return 0;
  }

  if (strcmp(command_name, "setcolor") == 0 ||
      strcmp(command_name, "color") == 0 || strcmp(command_name, "sc") == 0) {
    // if there is no tag, print error
    if (tags[0].tag[0] == '\0' || tags[0].value[0] == '\0') {
      uart_puts("\n\n");
      uart_puts("Invalid params for setcolor.\n");
      uart_puts("Usage: setcolor -t <text_color> -b <background_color>\n");
      uart_puts("Available colors: black, red, green, yellow, blue, magenta, "
                "cyan, white.\n\n");
      return 0;
    }

    set_color(tags);
    return 0;
  }

  if (strcmp(command_name, "config") == 0 ||
      strcmp(command_name, "conf") == 0 || strcmp(command_name, "cfg") == 0 ||
      strcmp(command_name, "cf") == 0) {
    // if there is no tag, print error
    if (tags[0].tag[0] == '\0' || tags[0].value[0] == '\0') {
      uart_puts("\n\n");
      uart_puts("Invalid params for config.\n");
      uart_puts("Usage: config -b <baudrate> -d <databits> -p <parity> -s "
                "<stopbits> -f <flow_control>\n");
      return 0;
    }

    set_config(tags);
    return 0;
  }

  if (strcmp(command_name, "image") == 0) {
    clear_frame_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);
    uart_puts("\n---Entering Image Mode---\n\n");
    uart_puts("Key to move the camera:\n");
    uart_puts("w: up\n");
    uart_puts("s: down\n");
    uart_puts("a: left\n");
    uart_puts("d: right\n\n");
    uart_puts("Press Escape to exit Image Mode\n");

    display_image(IMAGE_WIDTH, IMAGE_HEIGHT, epd_bitmap_image);
    is_mode_image = 1;
    return 0;
  }

  if (strcmp(command_name, "video") == 0) {
    clear_frame_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);
    uart_puts("\n---Entering Video Mode---\n\n");
    uart_puts("r: replay video\n\n");
    uart_puts("Press Escape to exit Video Mode\n");
    display_video(IMAGE_WIDTH, IMAGE_HEIGHT);
    is_mode_video = 1;
    return 0;
  }

  if (strcmp(command_name, "font") == 0) {
    clear_frame_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);
    uart_puts("\n---Entering Font Mode---\n\n");
    uart_puts("Press Escape to exit Font Mode\n");
    displayFont(IMAGE_WIDTH, IMAGE_HEIGHT);
    is_mode_font = 1;
    return 0;
  }

  if (strcmp(command_name, "play") == 0 || strcmp(command_name, "p") == 0) {
    // if there is no tag, print error
    if (tags[0].tag[0] == '\0' || tags[0].value[0] == '\0') {
      uart_puts("\n\n");
      uart_puts("Invalid params for play.\n");
      uart_puts("Usage: play -g <game> | Available games: unrob (ur)\n\n");
      return 0;
    }
    play_game(tags); // used this later on
    is_mode_game = 1;
    return 0;
  }

  // Find the command in the list of supported commands
  for (int i = 0; i < sizeof(commands) / sizeof(Command); i++) {
    // Check the name
    if (strcmp(command_name, commands[i].name) == 0) {
      new_line_with_color();
      commands[i].func();
      return 1;
    }

    // Check aliases
    char **alias = commands[i].aliases;
    while (*alias) {
      if (strcmp(command_name, *alias) == 0) {
        new_line_with_color();
        commands[i].func();
        return 1;
      }
      alias++;
    }
  }

  // Handle the case where the command is not found
  new_line_with_color();
  print_cmd_not_found();
  return 0;
}

void set_color(Tag *tags) {
  char text_color[10];
  char background_color[10];

  strcpy(text_color, OS_CONFIG.text_color);
  strcpy(background_color, OS_CONFIG.background_color);

  // tag can be -t for text color and -b for background color
  for (int i = 0; i < MAX_CMD_ARGS; i++) {
    // If the tag is empty or nullish, skip it
    if (tags[i].tag[0] == '\0' || tags[i].value[0] == '\0' ||
        tags[i].tag == (char *)0 || tags[i].value == (char *)0) {
      break;
    }

    if (strcmp(tags[i].tag, "t") == 0) {
      // reset the color
      for (int i = 0; i < 10; i++) {
        text_color[i] = '\0';
      }

      if (strcmp(tags[i].value, "black") == 0) {
        strcpy(text_color, COLOR.TEXT.BLACK);
      } else if (strcmp(tags[i].value, "red") == 0) {
        strcpy(text_color, COLOR.TEXT.RED);
      } else if (strcmp(tags[i].value, "green") == 0) {
        strcpy(text_color, COLOR.TEXT.GREEN);
      } else if (strcmp(tags[i].value, "yellow") == 0) {
        strcpy(text_color, COLOR.TEXT.YELLOW);
      } else if (strcmp(tags[i].value, "blue") == 0) {
        strcpy(text_color, COLOR.TEXT.BLUE);
      } else if (strcmp(tags[i].value, "magenta") == 0) {
        strcpy(text_color, COLOR.TEXT.MAGENTA);
      } else if (strcmp(tags[i].value, "cyan") == 0) {
        strcpy(text_color, COLOR.TEXT.CYAN);
      } else if (strcmp(tags[i].value, "white") == 0) {
        strcpy(text_color, COLOR.TEXT.WHITE);
      } else {
        uart_puts("\n\nInvalid text color.\n");
        uart_puts("Available colors: black, red, green, yellow, blue, magenta, "
                  "cyan, white.\n\n");

        return;
      }

      strcpy(OS_CONFIG.text_color, text_color);
    } else if (strcmp(tags[i].tag, "b") == 0) {
      // reset the color
      for (int i = 0; i < 10; i++) {
        background_color[i] = '\0';
      }

      if (strcmp(tags[i].value, "black") == 0) {
        strcpy(background_color, COLOR.BACKGROUND.BLACK);
      } else if (strcmp(tags[i].value, "red") == 0) {
        strcpy(background_color, COLOR.BACKGROUND.RED);
      } else if (strcmp(tags[i].value, "green") == 0) {
        strcpy(background_color, COLOR.BACKGROUND.GREEN);
      } else if (strcmp(tags[i].value, "yellow") == 0) {
        strcpy(background_color, COLOR.BACKGROUND.YELLOW);
      } else if (strcmp(tags[i].value, "blue") == 0) {
        strcpy(background_color, COLOR.BACKGROUND.BLUE);
      } else if (strcmp(tags[i].value, "magenta") == 0) {
        strcpy(background_color, COLOR.BACKGROUND.MAGENTA);
      } else if (strcmp(tags[i].value, "cyan") == 0) {
        strcpy(background_color, COLOR.BACKGROUND.CYAN);
      } else if (strcmp(tags[i].value, "white") == 0) {
        strcpy(background_color, COLOR.BACKGROUND.WHITE);
      } else {
        uart_puts("\n\nInvalid background color.\n");
        uart_puts("Available colors: black, red, green, yellow, blue, magenta, "
                  "cyan, white.\n\n");
        return;
      }

      strcpy(OS_CONFIG.background_color, background_color);
    } else {

      uart_puts("\n\nInvalid params for setcolor: -");

      uart_puts(tags[i].tag);
      uart_puts(" ");
      uart_puts(tags[i].value);

      uart_puts(".\n");
      uart_puts(
          "Available tags: -t for text color, -b for background color.\n\n");

      return;
    }
  }

  // Print success message
  new_line_force_color(text_color, background_color);
  format_and_print_string("Text color and background color have been set.",
                          text_color, background_color);
  new_line_force_color(text_color, background_color);
}

void set_config(Tag *tags) {
  int config_changed = 0;

  // tag can be -b for baudrate, -d for databits, -p for parity, -s for
  // stopbits, -f for flow control
  for (int i = 0; i < MAX_CMD_ARGS; i++) {
    // If the tag is empty or nullish, skip it
    if (tags[i].tag[0] == '\0' || tags[i].value[0] == '\0' ||
        tags[i].tag == (char *)0 || tags[i].value == (char *)0) {
      break;
    }

    if (strcmp(tags[i].tag, "b") == 0) {
      int baudrate = str2int(tags[i].value);

      if (baudrate == 0) {
        uart_puts("\n\nInvalid baudrate.\n");
        uart_puts("Baudrate must be an integer.\n\n");

        return;
      }

      if (uart_set_baudrate(baudrate, &BAUD_RATE_CONFIG) == 0) {
        uart_puts("\n\nBaudrate has been set.\n");

        config_changed = 1;
      }
    } else if (strcmp(tags[i].tag, "d") == 0) {
      int databits = str2int(tags[i].value);

      if (databits == 0) {
        uart_puts("\n\nInvalid databits.\n");
        uart_puts("Databits must be an integer.\n\n");

        return;
      }

      if (uart_set_databits(databits, &DBIT_CONFIG) == 0) {
        uart_puts("\n\nDatabits has been set.\n");

        config_changed = 1;
      }
    } else if (strcmp(tags[i].tag, "p") == 0) {
      int parity = str2int(tags[i].value);

      if (parity == 0) {
        uart_puts("\n\nInvalid parity.\n");
        uart_puts("Parity must be an integer.\n\n");

        return;
      }

      if (uart_set_parity(parity, &PARITY_MODE_CONFIG) == 0) {
        uart_puts("\n\nParity has been set.\n");

        config_changed = 1;
      }
    } else if (strcmp(tags[i].tag, "s") == 0) {
      int stopbits = str2int(tags[i].value);

      if (stopbits == 0) {
        uart_puts("\n\nInvalid stopbits.\n");
        uart_puts("Stopbits must be an integer.\n\n");

        return;
      }

      if (uart_set_stopbits(stopbits, &SBIT_CONFIG) == 0) {
        uart_puts("\n\nStopbits has been set.\n");

        config_changed = 1;
      }
    } else if (strcmp(tags[i].tag, "f") == 0) {
      int flow_control = str2int(tags[i].value);

      if (flow_control == 0) {
        uart_puts("\n\nInvalid flow control.\n");
        uart_puts("Flow control must be an integer.\n\n");

        return;
      }

      if (uart_set_flow_control(flow_control, &SEND_FLOW_CONFIG) == 0) {
        uart_puts("\n\nFlow control has been set.\n");

        config_changed = 1;
      }
    } else {

      uart_puts("\n\nInvalid params for config: -");

      uart_puts(tags[i].tag);
      uart_puts(" ");
      uart_puts(tags[i].value);

      uart_puts(".\n");

      uart_puts("Available tags: -b for baudrate, -d for databits, -p for "
                "parity, -s "
                "for stopbits, -f for flow control.\n\n");

      return;
    }
  }

  if (config_changed) {
    // Print success message
    new_line_with_color();

    uart_puts("Proceeding to reload uart...\n");

    // Restart the uart
    restart_uart();
  }
}

void play_game(Tag tags[MAX_CMD_ARGS]) {
  // tag can be -g for game
  for (int i = 0; i < MAX_CMD_ARGS; i++) {
    // If the tag is empty or nullish, skip it
    if (tags[i].tag[0] == '\0' || tags[i].value[0] == '\0' ||
        tags[i].tag == (char *)0 || tags[i].value == (char *)0) {
      break;
    }

    if (tags[i].tag == (char *)0 || tags[i].value == (char *)0) {
      uart_puts("\nNo tag supplied.\n");
      uart_puts("Available tags: -g for game.\n\n");
      return;
    }

    if (strcmp(tags[i].tag, "g") == 0) {
      if (strcmp(tags[i].value, "unrob") == 0 ||
          strcmp(tags[i].value, "ur") == 0) {
        start_unrob_game();
      } else if (strcmp(tags[i].value, "breakout") == 0 ||
                 strcmp(tags[i].value, "bk") == 0) {
        start_breakout_game();
      } else {
        uart_puts("\nInvalid game.\n");
        uart_puts("Available games: unrob (ur).\n\n");
        return;
      }
    } else {
      uart_puts("\nInvalid params for play.\n");
      uart_puts("Available tags: -g for game.\n\n");
      return;
    }
  }
}

void save_command(char *command, CommandHistory *cmd_history,
                  int *past_cmd_index) {
  // If the command is empty, do not save it
  if (strcmp(command, "") == 0) {
    return;
  }

  // If the command is the same as the last command, do not save it
  if (cmd_history->size >= 0 &&
      strcmp(command, cmd_history->commands[cmd_history->size]) == 0) {
    return;
  }

  // Check if the stack is full
  if (cmd_history->size == MAX_PAST_CMD - 1) {
    // Shift the stack to the left
    for (int i = 0; i < MAX_PAST_CMD - 1; i++) {
      strcpy(cmd_history->commands[i], cmd_history->commands[i + 1]);
    }
    // Decrement the size to account for the shifted commands
    cmd_history->size--;
  }

  // Increment the top index
  cmd_history->size++;
  *past_cmd_index = cmd_history->size + 1;

  // Copy the command to the top of the stack
  trim(command);
  strcpy(cmd_history->commands[cmd_history->size], command);
}

void autofill_command(char *buffer, char *completed_command,
                      char *pre_autofilled_cmd, char *post_autofilled_cmd) {
  int start_index = 0;
  int command_count = 0;

  // get the number of commands
  for (int i; commands[command_count].name != (char *)0; i++)
    command_count++;

  if (post_autofilled_cmd && strlen(post_autofilled_cmd) > 0) {
    // find the index of the command
    for (int i = 0; i < command_count; i++) {
      if (strcmp(commands[i].name, post_autofilled_cmd) == 0) {
        start_index = i + 1;
        break;
      }
    }

    // reset the start index if it is greater than the command count
    if (start_index == command_count) {
      start_index = 0;
    }
  }

  // go through command names
  for (int i = start_index; i < start_index + command_count; i++) {
    int index = i % command_count;
    if (has_prefix(commands[index].name, pre_autofilled_cmd)) {
      strcpy(completed_command, commands[index].name);
      strcpy(post_autofilled_cmd, commands[index].name);
      return;
    }
  }

  // go through command aliases
  for (int i = start_index; i < start_index + command_count; i++) {
    int index = i % command_count;
    char **alias = commands[index].aliases;
    while (*alias) {
      if (has_prefix(*alias, pre_autofilled_cmd)) {
        strcpy(completed_command, *alias);
        strcpy(post_autofilled_cmd, *alias);
        return;
      }
      alias++;
    }
  }

  // if no command or alias is found, return the buffer
  strcpy(completed_command, buffer);
}

void restart_uart() {
  uart_puts("\nUART0 is restarting...\n");
  uart_init(1);
}