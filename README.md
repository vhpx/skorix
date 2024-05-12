# **Skorix OS**

A bare-metal operating system designed for the Raspberry Pi, emphasizing clarity, efficiency, and a focus on the Command Line Interface (CLI).

## **Project Structure**

The project is organized into the following directories:

```
. <--- (root directory)
|
+---Makefile
|       
+---src
    +---build
    |   |   *.o files
    |   |   
    |   +---images
    |   |       kernel8.img
    |   |       
    |   +---kernel
    |           kernel8.elf
    |           
    +---lib
    |   +---core
    |   |       
    |   +---games
    |   |   |
    |   |   +---unrob
    |   |   |
    |   |   +---engine
    |   |       
    |   +---headers
    |   |       *.h files
    |   |       
    |   +---utils
    |           
    +---os
        +---kernel
        |       
        +---mailbox
        |
        +---uarts
```

## **Features**

* **Minimalist Welcome Message:** Provides essential boot information.
* **Basic Commands:**  Includes `help`, `clear`, `setcolor`, and `showinfo`.
* **Command Aliases:** Streamline interaction with shortcuts like `h` for `help`
* **Quality of Life CLI Enhancements:**
     * Command completion (TAB key)
     * Command history navigation (underscore and plus keys)
     * Colored text output using ANSI escape characters
     * Structured table formatting for improved readability
* **Utility Commands:** `echo`, `shutdown` (prototype), `reload`, `history`, `about`
* **UART Driver Development:** Enhanced functionality within the UART driver

## **Getting Started**

1. **Prerequisites:**
    * Raspberry Pi board (Model 3, 4 or compatible)
    * QEMU Raspberry Pi 3 emulator (for testing)
    * Toolchain for ARM (e.g., GCC, Make, etc.)
2. **Build and Run:**
    * Run the `make` command to compile the source code.
    * QEMU will automatically launch the OS for testing.
    * To run on a Raspberry Pi, copy the `kernel8.img` file to an SD card.

## **Customization**

Use the `setcolor` command to personalize text and background colors within the CLI. Experiment with different preset colors to find your preferred scheme.

## **Future Development**

* Advanced command-line editing capabilities
* File system support
* Process management
* More sophisticated shutdown implementation
* Exploration of GUI elements

# **About the Authors**

Skorix OS was initially developed by [**Vo Hoang Phuc**](https://github.com/vhpx) (S3926761) as part of the Embedded Systems: OS and Interfacing (EEET2490) course at RMIT University. The project aims to provide a foundational understanding of operating system development and low-level programming.

The project is then further developed in collaboration with [**Do Phuong Linh**](https://github.com/dophuonglinhh), [**Le Duy Quang**](https://github.com/leQwang), and [**Tran Thanh Tung**](https://github.com/s3927562) to enhance the OS's functionality and user experience. We've worked together to implement image, video, and custom font display capabilities, as well as implementing the "Unrob" game (a clone of the ["Unthief" game by Dan or Daniel](https://dan-or-daniel.itch.io/unthief)). The game utilized the UART driver to receive user input and display the game state on the screen, while having custom timer and interrupt handling to ensure smooth gameplay.

## **Authors**

* [**Vo Hoang Phuc**](https://github.com/vhpx)
* [**Do Phuong Linh**](https://github.com/dophuonglinhh)
* [**Le Duy Quang**](https://github.com/leQwang)
* [**Tran Thanh Tung**](https://github.com/s3927562)

## **Contact**

For questions, feedback, or contributions, please reach out to s3926761@rmit.edu.vn, or contact@vohoangphuc.com.