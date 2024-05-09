# Compiler and flags
CC = aarch64-none-elf-gcc
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib

# Directories
SRC_DIR = ./src
BUILD_DIR = ./build
KERNEL_DIR = $(SRC_DIR)/os/kernel
LIB_DIR = $(SRC_DIR)/lib
IMAGE_DIR = $(SRC_DIR)/img
VIDEO_DIR = $(SRC_DIR)/video
FONT_DIR = $(SRC_DIR)/font


# System and library files
KERNEL_CFILES = $(wildcard $(KERNEL_DIR)/*.c)
CORE_LIB_CFILES = $(wildcard $(LIB_DIR)/core/*.c)
MBOX_LIB_CFILES = $(wildcard $(LIB_DIR)/mailbox/*.c)
UTILS_LIB_CFILES = $(wildcard $(LIB_DIR)/utils/*.c)

# Media Processing files
IMAGE_CFILES = $(wildcard $(IMAGE_DIR)/*.c)
VIDEO_CFILES = $(wildcard $(VIDEO_DIR)/*.c)
FONT_CFILES = $(wildcard $(FONT_DIR)/*.c)

# Game Engine files
GENGINE_LIB_CFILES = $(wildcard $(LIB_DIR)/games/engine/*.c)
UNROB_GAME_LIB_CFILES = $(wildcard $(LIB_DIR)/games/unrob/*.c)

# System files
CFILES = $(KERNEL_CFILES) $(CORE_LIB_CFILES) $(MBOX_LIB_CFILES) $(UTILS_LIB_CFILES) $(IMAGE_CFILES) $(VIDEO_CFILES) $(FONT_CFILES) $(GENGINE_LIB_CFILES) $(UNROB_GAME_LIB_CFILES)
OFILES = $(addprefix $(BUILD_DIR)/, $(notdir $(CFILES:%.c=%.o)))

# Targets
UART = uart0
TARGET = kernel8.img
QEMU = qemu-system-aarch64

Force cmd on Windows
ifeq ($(OS), Windows_NT)
	SHELL = cmd
endif

#----------------------------------------
# Main Rules
#----------------------------------------

all: mk_dirs clean build $(TARGET) run0

fast: mk_dirs clean_fast build $(TARGET) run0

fast: mk_dirs clean_fast build $(TARGET) run0

mac: mk_dirs_mac clean_mac build $(TARGET) run0_mac

mac_fast: mk_dirs_mac clean_mac_fast build $(TARGET) run0_mac

mac_fast: mk_dirs_mac clean_mac_fast build $(TARGET) run0_mac

$(TARGET): $(BUILD_DIR)/boot.o $(BUILD_DIR)/uart.o $(OFILES)
	aarch64-none-elf-ld -nostdlib $^ -T $(KERNEL_DIR)/link.ld -o $(BUILD_DIR)/kernel/kernel8.elf
	aarch64-none-elf-objcopy -O binary $(BUILD_DIR)/kernel/kernel8.elf $(BUILD_DIR)/images/$(TARGET)

#----------------------------------------
# Utility Rules 
#----------------------------------------

mk_dirs:
	if not exist .\build mkdir .\build
	if not exist .\build\kernel mkdir .\build\kernel
	if not exist .\build\images mkdir .\build\images

mk_dirs_mac:
	if [ ! -d ./build ]; then mkdir ./build; fi
	if [ ! -d ./build/kernel ]; then mkdir ./build/kernel; fi
	if [ ! -d ./build/images ]; then mkdir ./build/images; fi

clean:
#   Remove old build files (./src/build)
	if exist .\src\build\*.o del .\src\build\*.o
	if exist .\src\build\kernel\*.elf del .\src\build\kernel\*.elf
	if exist .\src\build\images\*.img del .\src\build\images\*.img

#   Remove old build directories (./src/build)
	if exist .\src\build\kernel rmdir .\src\build\kernel /s /q
	if exist .\src\build\images rmdir .\src\build\images /s /q
	if exist .\src\build rmdir .\src\build /s /q

#   Remove new build files (./build)
	if exist .\build\*.o del .\build\*.o
	if exist .\build\kernel\*.elf del .\build\kernel\*.elf
	if exist .\build\images\*.img del .\build\images\*.img

clean_mac:
#   Remove old build files (./src/build)
	rm -f ./src/build/*.o
	rm -f ./src/build/kernel/*.elf
	rm -f ./src/build/images/*.img

#   Remove old build directories (./src/build)
	rm -rf ./src/build/kernel
	rm -rf ./src/build/images
	rm -rf ./src/build
	
#   Remove new build files (./build)
	rm -f ./build/*.o
	rm -f ./build/kernel/*.elf
	rm -f ./build/images/*.img

clean_fast:
	for %%f in (.\build\*.o) do if not %%~nxf == video_src.o (if not %%~nxf == img_src.o del %%~f)
	if exist .\build\kernel\*.elf del .\build\kernel\*.elf
	if exist .\build\images\*.img del .\build\images\*.img

clean_mac_fast:
	find -iname '*.o' -not -iname 'video_src.o' -not -iname 'img_src.o' -execdir rm {} \;
	rm -f ./build/kernel/*.elf
	rm -f ./build/images/*.img

#----------------------------------------
# Compilation & Linking
#----------------------------------------

build: $(OFILES)

$(BUILD_DIR)/boot.o: $(KERNEL_DIR)/boot.S
	$(CC) $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/uart.o: ./src/os/uarts/$(UART).c
	$(CC) $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c
	$(CC) $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/core/%.c
	$(CC) $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/mailbox/%.c
	$(CC) $(GCCFLAGS) -c $< -o $@	

$(BUILD_DIR)/%.o: $(LIB_DIR)/utils/%.c
	$(CC) $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(IMAGE_DIR)/%.c
	$(CC) $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(VIDEO_DIR)/%.c
	$(CC) $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(FONT_DIR)/%.c
	$(CC) $(GCCFLAGS) -c $< -o $@	

$(BUILD_DIR)/%.o: $(LIB_DIR)/games/engine/%.c
	$(CC) $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/games/unrob/%.c
	$(CC) $(GCCFLAGS) -c $< -o $@

#----------------------------------------
# Emulation
#----------------------------------------

run1: $(TARGET)
	$(QEMU) -M raspi3 -kernel $(BUILD_DIR)/images/$(TARGET) -serial null -serial stdio

run0: $(TARGET) 
	$(QEMU) -M raspi3 -kernel $(BUILD_DIR)/images/$(TARGET) -serial stdio

run1_mac: $(TARGET)
	$(QEMU) -M raspi3b -kernel $(BUILD_DIR)/images/$(TARGET) -serial null -serial stdio

run0_mac: $(TARGET) 
	$(QEMU) -M raspi3b -kernel $(BUILD_DIR)/images/$(TARGET) -serial stdio
