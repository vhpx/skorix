// this file is cli.c
#include "../headers/cli.h"

#include "../../img/img.h"
#include "../../img/img_src.h"
#include "../../img/welcome_img.h"
#include "../../video/video.h"
#include "../games/screens/game-menu.h"
#include "../games/unrob/game.h"
#include "../headers/color.h"
#include "../headers/config.h"
#include "../headers/constants.h"
#include "../headers/exception.h"
#include "../headers/framebf.h"
#include "../headers/interrupt.h"
#include "../headers/print.h"
#include "../headers/string.h"
#include "../headers/timer.h"
#include "../headers/uart0.h"

// TODO: Reset to CLI after the game is done
int mode = GAME;

// int is_game_start = 0;

int cli(char c) {
  static char cli_buffer[MAX_CMD_SIZE];
  static int index = 0;

  static int past_cmd_index = -1;
  static char pre_autofilled_cmd[MAX_CMD_SIZE];
  static char post_autofilled_cmd[MAX_CMD_SIZE];

  static CommandHistory cmd_history = {
      .commands = {},
      .size = -1,
  };
  // read and send back each char
  //   char c = uart_getc();
  int shutdown =
      handle_input(c, cli_buffer, &index, &past_cmd_index, &cmd_history,
                   pre_autofilled_cmd, post_autofilled_cmd);

  if (shutdown == -1) {
    return -1;
  }

  return 0;
}

int run_cli() {
  // Initialize the console
  setup_console();
  clear_screen();
  print_welcome_message();
  reset_console();

  // Enable interrupts
  exception_init();
  interrupt_init();
  sys_timer1_init();
  sys_timer1_irq_enable();
  uart0_irq_enable();
  interrupt_enable();

  // Initialize the frame buffer
  initialize_frame_buffer(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH,
                          SCREEN_HEIGHT);

  // TODO: Remove this after the game is done
  if (mode == GAME)
    game_start_selector();
  else
    display_image(SCREEN_WIDTH, SCREEN_HEIGHT, welcome_img);

  // Start the CLI
  //   int status = 0;
  //   while (status != -1) {
  //     status = cli();

  //     // Break if the status is -1
  //     if (status == -1)
  //       break;
  //   }

  //   return status;

  while (1) {
    if (mode == SHUTDOWN) {
      break;
    }
  }

  return mode;
}

int handle_input(char c, char *cli_buffer, int *index, int *past_cmd_index,
                 CommandHistory *cmd_history, char *pre_autofilled_cmd,
                 char *post_autofilled_cmd) {
  int reset_past_cmd_index = 1;
  char2lower(&c);

  // TODO: Add improved support for image scrolling
  if (mode == IMAGE) {
    if (c == 'w' || c == 's' || c == 'a' || c == 'd') {
      scroll_image(c, IMAGE_WIDTH, IMAGE_HEIGHT, epd_bitmap_image);
    } else if (c == 27) { // escape key
      // exit all the modes
      clear_frame_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);
      uart_puts("\n\nExiting image view...\n\n");
      mode = CLI;
      reset_console();
      display_image(SCREEN_WIDTH, SCREEN_HEIGHT, welcome_img);
    }
  } else if (mode == VIDEO) {
    if (c == 'r') {
      if (video_end) {
        display_video(IMAGE_WIDTH, IMAGE_HEIGHT);
      } else {
        video_restart = 1;
      }
    } else if (c == 'p') {
      if (video_pause) {
        video_pause = 0;
      } else {
        video_pause = 1;
      }
    } else if (c == 27) { // escape key
      video_exit = 1;
      uart_puts("\n\nExiting video playback...\n\n");
      mode = CLI;
      reset_console();
      display_image(SCREEN_WIDTH, SCREEN_HEIGHT, welcome_img);
    }

  } else if (mode == FONT) {
    if (c == 27) { // escape key
      clear_frame_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);
      uart_puts("\n\nExiting font view...\n\n");
      mode = CLI;
      reset_console();
      display_image(SCREEN_WIDTH, SCREEN_HEIGHT, welcome_img);
    }

  } else if (mode == GAME) {
    if (c == 'r') {
      handle_received_key(c);
      uart_puts("\nRestarting Unrob Game...\n");
      clear_frame_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);
      sys_timer3_irq_disable();
      start_unrob_game();
      uart_puts("Restarted Unrob Game.");
      return 0;
    }

    if (!is_game_start) {
      if (c == 'w' || c == 's') {
        handle_received_key(c);
        uart_puts("\nSelecting option: ");
        uart_puts(COLOR.TEXT.YELLOW);
        uart_puts(c == 'w' ? "Start" : "Quit");
        uart_puts(COLOR.RESET);

        select_game_start_exit(c);
        return 0;
      }

      if (c != 27 && c != '\n') {
        handle_invalid_key(c);
        return 0;
      }

      handle_received_key(c);
      if (select_game_option) {
        if (c == 27) { // escape key
          exit_game();
          return 0;
        }

        is_game_start = 1;

        uart_puts("\nSelecting Level: ");
        uart_puts(COLOR.TEXT.YELLOW);
        uart_dec(selected_level);
        uart_puts(COLOR.RESET);

        level_selector();
      } else {
        exit_game();
      }

      return 0;
    }

    if (!is_level_selected) {
      if (c == 27) { // escape key
        handle_received_key(c);
        uart_puts("\nReturning to Game Menu...\n");
        select_game_start_exit('w');
        is_game_over = 0;
        is_game_start = 0;
        selected_level = 1;
        is_level_selected = 0;
        return 0;
      }

      if (c == 'w' || c == 's' || c == '\n') {
        handle_received_key(c);

        select_level(c);

        if (c == '\n') {
          is_level_selected = 1;
        } else {
          uart_puts("\nSelecting Level: ");
          uart_puts(COLOR.TEXT.YELLOW);
          uart_dec(selected_level);
          uart_puts(COLOR.RESET);
        }
      } else {
        handle_invalid_key(c);
      }
    } else {
      if (c == 27) { // escape key
        handle_received_key(c);
        uart_puts("\nReturning to Level Selection...\n");
        level_selector();
        is_game_over = 0;
        is_game_start = 1;
        selected_level = 1;
        is_level_selected = 0;
        return 0;
      }

      if (c == 'w' || c == 's' || c == 'a' || c == 'd') {
        handle_received_key(c);
        move_player(c);
      } else if (c == 'q' || c == 'e') {
        handle_received_key(c);
        switch_inventory_item(c);
      } else if (c == 'f') {
        handle_received_key(c);
        execute_main_action();
      } else if (c == 'c') {
        handle_received_key(c);
        uart_puts("\nGame Debug Mode: ");
        uart_puts(enable_game_debugger ? COLOR.TEXT.RED : COLOR.TEXT.GREEN);
        uart_puts(enable_game_debugger ? "OFF" : "ON");
        uart_puts(COLOR.RESET);
        toggle_game_debugger();
      } else if (c == 'x') {
        handle_received_key(c);
        uart_puts("\nRendering Debug Mode: ");
        uart_puts(enable_rendering_debugger ? COLOR.TEXT.RED
                                            : COLOR.TEXT.GREEN);
        uart_puts(enable_rendering_debugger ? "OFF" : "ON");
        uart_puts(COLOR.RESET);
        toggle_rendering_debugger();
      } else if (c == 'h') {
        handle_received_key(c);
        display_game_controls();
      } else {
        handle_invalid_key(c);
      }
    }
  } else if (c == '\b' || c == 0x7F) {
    handle_backspace(cli_buffer, index, pre_autofilled_cmd,
                     post_autofilled_cmd);
  } else if (c == '\t') {
    handle_autocomplete(cli_buffer, index, pre_autofilled_cmd,
                        post_autofilled_cmd);
  } else if (c == '+' || c == '_') {
    handle_history_navigation(c, cli_buffer, index, past_cmd_index,
                              cmd_history);
    reset_past_cmd_index = 0;

    strcpy(pre_autofilled_cmd, cli_buffer);
    strcpy(post_autofilled_cmd, cli_buffer);

  } else if (c != '\n') {
    handle_regular_input(c, cli_buffer, index, pre_autofilled_cmd,
                         post_autofilled_cmd);
  } else if (c == '\n') {
    int shutdown =
        handle_newline(cli_buffer, index, past_cmd_index, cmd_history);

    reset_past_cmd_index = 0;

    if (shutdown == -1) {
      return -1;
    }

    // Reset the pre-autofilled and post-autofilled commands
    clrstr(pre_autofilled_cmd);
    clrstr(post_autofilled_cmd);
  }

  if (reset_past_cmd_index) {
    *past_cmd_index = cmd_history->size + 1;
  }

  return 0;
}

void exit_game() {
  clear_frame_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);
  sys_timer3_irq_disable();
  uart_puts("\n\nExiting game...\n\n");
  mode = CLI;
  reset_console();
  display_image(SCREEN_WIDTH, SCREEN_HEIGHT, welcome_img);
  is_level_selected = 0;
  is_game_start = 0;
  is_game_over = 0;
  selected_level = 1;
}

void handle_backspace(char *cli_buffer, int *index, char *pre_autofilled_cmd,
                      char *post_autofilled_cmd) {
  if (*index > 0) {
    (*index)--;
    cli_buffer[*index] = '\0';
    strcpy(post_autofilled_cmd, cli_buffer);
    strcpy(pre_autofilled_cmd, cli_buffer);
    uart_puts("\b \b");
  }
}

void handle_autocomplete(char *cli_buffer, int *index, char *pre_autofilled_cmd,
                         char *post_autofilled_cmd) {
  char completed_command[MAX_CMD_SIZE];

  // Clear completion buffer
  clrstr(completed_command);

  // Get the completed command
  autofill_command(cli_buffer, completed_command, pre_autofilled_cmd,
                   post_autofilled_cmd);

  // Copy the completed command to the buffer
  reset_console();
  strcpy(cli_buffer, completed_command);
  strcpy(post_autofilled_cmd, completed_command);
  *index = strlen(cli_buffer);
  format_and_print_string(cli_buffer, OS_CONFIG.text_color,
                          OS_CONFIG.background_color);
}

void handle_history_navigation(char c, char *cli_buffer, int *index,
                               int *past_cmd_index,
                               CommandHistory *cmd_history) {
  if (*past_cmd_index == -1)
    *past_cmd_index = cmd_history->size;

  reset_console();
  if (c == '_' && *past_cmd_index > 0) {
    // Move up in history (towards more recent commands)
    (*past_cmd_index)--;
  } else if (c == '+' && *past_cmd_index <= cmd_history->size) {
    // Move down in history (towards older commands)
    (*past_cmd_index)++;
  }

  // Copy the command from the history to the buffer
  strcpy(cli_buffer, cmd_history->commands[*past_cmd_index]);
  *index = strlen(cli_buffer);

  format_and_print_string(cli_buffer, OS_CONFIG.text_color,
                          OS_CONFIG.background_color);
}

void handle_regular_input(char c, char *cli_buffer, int *index,
                          char *pre_autofilled_cmd, char *post_autofilled_cmd) {
  if (*index >= MAX_CMD_SIZE) {
    uart_puts("\nError: Command size exceeds maximum limit of ");

    char size[10];
    int2str(MAX_CMD_SIZE, size);
    uart_puts(size);
    uart_puts(" characters\n\n");

    reset_console();
    uart_puts(cli_buffer);

    return;
  }

  // Append the character to the command buffer and autofilled command
  cli_buffer[*index] = c;
  pre_autofilled_cmd[*index] = c;
  strcpy(post_autofilled_cmd, pre_autofilled_cmd);

  // Update the index
  (*index)++;

  char str[2] = {c, '\0'};
  format_and_print_string(str, OS_CONFIG.text_color,
                          OS_CONFIG.background_color);
}

int handle_newline(char *cli_buffer, int *index, int *past_cmd_index,
                   CommandHistory *cmd_history) {
  cli_buffer[*index] = '\0';

  // Save the command to the history
  save_command(cli_buffer, cmd_history, past_cmd_index);
  int has_cmd = execute_command(cli_buffer, cmd_history);
  if (mode == CLI)
    reset_console();

  // Shutdown the system if the command is exit
  if (has_cmd == -1) {
    uart_puts("shutdown\n");
    uart_puts("\nShutting down...\nGoodbye!\n\n");
  }

  *index = 0;
  clrstr(cli_buffer);

  return has_cmd;
}
