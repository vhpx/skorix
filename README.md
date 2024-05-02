# **Skorix OS**

A bare-metal operating system designed for the Raspberry Pi, emphasizing clarity, efficiency, and a focus on the Command Line Interface (CLI).

## **Project Structure**

The project is organized into the following directories:

```
. <--- (root directory)
|
|   .gitignore
|   Makefile
|   README.md
|   tree.txt
|   
+---.vscode
|       settings.json
|       
+---docs
|       specifications.pdf
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
    |   |       cli.c
    |   |       
    |   +---headers
    |   |       *.h files
    |   |       
    |   +---mailbox
    |   |       mbox.c
    |   |       
    |   +---utils
    |           color.c
    |           command.c
    |           config.c
    |           constants.c
    |           number.c
    |           print.c
    |           string.c
    |           
    +---os
        +---kernel
        |       boot.S
        |       kernel.c
        |       link.ld
        |       
        +---uarts
                uart0.c
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

# **About the Author**

Skorix OS was developed by Vo Hoang Phuc (S3926761) as part of the Embedded Systems: OS and Interfacing (EEET2490) course at RMIT University. The project aims to provide a foundational understanding of operating system development and low-level programming.

## **Contact**

For questions, feedback, or contributions, please reach out to s3926761@rmit.edu.vn, or contact@vohoangphuc.com.