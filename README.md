<img src="/public/skx.png" alt="Skorix OS" style="width:100%;">

# **Skorix OS**

A bare-metal operating system designed for the Raspberry Pi, emphasizing clarity, efficiency, and a focus on the Command Line Interface (CLI). This project extends the initial development by incorporating multimedia functionalities and a fully functional interactive game.

## Key Features

* **Minimalist Welcome Message:** Provides essential boot information.
* **Basic Commands:**  `help`, `clear`, `setcolor`, `showinfo`
* **Command Aliases:** Shortcuts for ease of use (e.g., `h` for `help`)
* **Enhanced CLI Experience:**
     * Command completion (TAB key)
     * Command history (underscore and plus keys)
     * Colored text output (ANSI escape codes)
     * Table formatting for clear data presentation 
* **Utility Commands:** `echo`, `shutdown` (prototype), `reload`, `history`, `about`
* **UART Driver:** Improved UART driver for enhanced communication
* **Multimedia Capabilities:**
    * **Image Display and Scrolling:** Run `image` to view and navigate large images.
    * **Video Playback:** Run `video` to play, pause, and replay short demo videos.
    * **Dynamic Font Rendering:** Run `team` to display our team members on the screen using custom font rendering. This feature is also heavily utilized in our custom-made "Unrob" game for text display.
* **CLI Helper Message:** Extensive helper messages are implemented for ease of navigation.
* **Timer and Interrupt Integration:** The game uses a timer for countdowns and interrupts to handle game exit/game over scenarios.
* **Interactive Game - "Unrob":**
    * **Engaging Gameplay:**  Help the thief return stolen items while avoiding guards.
    * **Multiple Stages:** Navigate different museum layouts with varying challenges.
    * **Timer and Interrupt Integration:** Time limits and dynamic enemy movement.
    * Access the game by running `play -g unrob`. Game controls are as follows:
        * **W, A, S, D:** Move the player.
        * **Q, E:** Navigate through the inventory.
        * **F:** Place or swap the item.
        * **R:** Restart the game.
        * **C:** Toggle Debug Mode (contains spoilers).
        * **X:** Toggle Rendering Debug Mode.
        * **H:** Display the help message.
        * **Esc:** Quit the game.


## Getting Started

1. **Prerequisites:**
    * Raspberry Pi board (Model 3, 4, or compatible)
    * QEMU Raspberry Pi 3 emulator (for testing)
    * ARM Toolchain (e.g., GCC, Make) 
2. **Build and Run:**
    * Execute `make` to compile the source code.
    * QEMU automatically launches the OS for testing.
    * Copy `kernel8.img` to an SD card for running on a Raspberry Pi.

## **Project Structure**

```
.
├── .gitignore
├── Makefile
├── ...
├── src
│   ├── font
│   │   ├── font.c
│   │   ├── members_img.c
│   │   └── ...
│   ├── img
│   │   ├── img.c
│   │   ├── img.h
│   │   ├── img_src.c
│   │   └── ...
│   ├── lib
│   │   ├── core
│   │   │   └── cli.c
│   │   ├── games
│   │   │   ├── engine
│   │   │   │   ├── gengine.c
│   │   │   │   ├── guard.c
│   │   │   │   └── ... 
│   │   │   ├── screens
│   │   │   │   ├── game-menu.c
│   │   │   │   ├── post-game-buttons.c
│   │   │   │   └── ...
│   │   │   └── unrob
│   │   │       ├── game.c
│   │   │       └── ...
│   │   ├── headers
│   │   │   ├── cli.h
│   │   │   ├── color.h
│   │   │   └── ...
│   │   └── utils
│   │       ├── color.c
│   │       ├── command.c
│   │       └── ...
│   ├── os
│   │   ├── kernel
│   │   │   ├── boot.S
│   │   │   └── ...
│   │   ├── mailbox
│   │   │   └── mbox.c
│   │   └── uarts
│   │       └── uart0.c
│   └── video
│       ├── video.c
│       ├── video.h
│       └── ...
└── ...
```


## **Future Development**

* Advanced command-line editing features
* File system implementation
* Process management capabilities
* Networking and internet connectivity
* Enhanced multimedia capabilities
* Additional games and interactive applications

## **About the Project**

Skorix OS was initially developed by [**Vo Hoang Phuc**](https://github.com/vhpx) as part of the Embedded Systems: OS and Interfacing (EEET2490) course at RMIT University (legacy source code available [here](https://github.com/vhpx/skorix-legacy)). The project aims to provide a foundational understanding of operating system development and low-level programming.

The project is then further developed in collaboration with [**Do Phuong Linh**](https://github.com/dophuonglinhh), [**Le Duy Quang**](https://github.com/leQwang), and [**Tran Thanh Tung**](https://github.com/s3927562) to enhance the OS's functionality and user experience. We've worked together to implement image, video, and custom font display capabilities, as well as implementing the "Unrob" game (a clone of the ["Unthief" game by Dan or Daniel](https://dan-or-daniel.itch.io/unthief)). The game utilized the UART driver to receive user input and display the game state on the screen, while having custom timer and interrupt handling to ensure smooth gameplay and user interaction.

## **Authors**

* [**Vo Hoang Phuc**](https://github.com/vhpx) (S3926761)
* [**Do Phuong Linh**](https://github.com/dophuonglinhh) (S3926823)
* [**Le Duy Quang**](https://github.com/leQwang) (S3912105)
* [**Tran Thanh Tung**](https://github.com/s3927562) (S3927562)

This collaboration focused on integrating multimedia features, exploring Raspberry Pi's hardware capabilities (timers and interrupts), and developing the interactive game "Unrob."

## **Contact**

For inquiries, feedback, or contributions, please reach out to s3926761@rmit.edu.vn, or contact@vohoangphuc.com.
