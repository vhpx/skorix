# Compiler and flags
CC = aarch64-none-elf-gcc
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib

# Directories
SRC_DIR = ./src
BUILD_DIR = ./build
OS_DIR = $(SRC_DIR)/os
KERNEL_DIR = $(OS_DIR)/kernel
LIB_DIR = $(SRC_DIR)/lib
IMAGE_DIR = $(SRC_DIR)/img
VIDEO_DIR = $(SRC_DIR)/video
FONT_DIR = $(SRC_DIR)/font


# System and library files
KERNEL_CFILES = $(wildcard $(KERNEL_DIR)/*.c)
MBOX_LIB_CFILES = $(wildcard $(OS_DIR)/mailbox/*.c)

CORE_LIB_CFILES = $(wildcard $(LIB_DIR)/core/*.c)
UTILS_LIB_CFILES = $(wildcard $(LIB_DIR)/utils/*.c)

# Media Processing files
IMAGE_CFILES = $(wildcard $(IMAGE_DIR)/*.c)
VIDEO_CFILES = $(wildcard $(VIDEO_DIR)/*.c)
FONT_CFILES = $(wildcard $(FONT_DIR)/*.c)

# Game Engine files
GENGINE_LIB_CFILES = $(wildcard $(LIB_DIR)/games/engine/*.c)
UNROB_GAME_LIB_CFILES = $(wildcard $(LIB_DIR)/games/unrob/*.c)

# C System files
CFILES = $(KERNEL_CFILES) $(CORE_LIB_CFILES) $(MBOX_LIB_CFILES) $(UTILS_LIB_CFILES) $(IMAGE_CFILES) $(VIDEO_CFILES) $(FONT_CFILES) $(GENGINE_LIB_CFILES) $(UNROB_GAME_LIB_CFILES)
OFILES = $(addprefix $(BUILD_DIR)/, $(notdir $(CFILES:%.c=%.o)))

# Assembly System files
SFILES = $(wildcard $(LIB_DIR)/utils/*.S)
OFILES += $(addprefix $(BUILD_DIR)/, $(notdir $(SFILES:%.S=%_asm.o)))

# Targets
UART = uart0
TARGET = kernel8.img
QEMU = qemu-system-aarch64

# RPI Version
ifneq ($(pi), 4)
	GCCFLAGS += -DRPI3
endif

# Force cmd on Windows
ifeq ($(OS), Windows_NT)
	SHELL = cmd
	RMIMG = if exist .\build\img_src.o del .\build\img_src.o
	RMVIDEO = if exist .\build\video_src.o del .\build\video_src.o
	RMMAP = if exist .\build\map-bitmap.o del .\build\map-bitmap.o
	RMWELCOME = if exist .\build\welcome_img.o del .\build\welcome_img.o
	RMGAMEMENU = if exist .\build\game-menu.o del .\build\game-menu.o
else
	RMIMG = rm -f ./build/img_src.o
	RMVIDEO = rm -f ./build/video_src.o
	RMMAP = rm -f ./build/map-bitmap.o
	RMWELCOME = rm -f ./build/welcome_img.o
	RMGAMEMENU = rm -f ./build/game-menu.o
endif

#----------------------------------------
# Main Rules
#----------------------------------------

all: mk_dirs clean build $(TARGET) run0

full: mk_dirs clean clean_img clean_video build $(TARGET) run0

image: mk_dirs clean clean_img build $(TARGET) run0

video: mk_dirs clean clean_video build $(TARGET) run0

mac: mk_dirs_mac clean_mac build $(TARGET) run0_mac

macfull: mk_dirs_mac clean_mac clean_img clean_video build $(TARGET) run0_mac

macimage: mk_dirs_mac clean_mac clean_img build $(TARGET) run0_mac

macvideo: mk_dirs_mac clean_mac clean_video build $(TARGET) run0_mac

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
#   Remove old build files (.\src\build)
	if exist .\src\build rmdir .\src\build /s /q

#   Remove new build files (.\build)
	for %%f in (.\build\*.o) do if not %%~nxf == video_src.o (if not %%~nxf == img_src.o (if not %%~nxf == map-bitmap.o (if not %%~nxf == welcome_img.o (if not %%~nxf == game-menu.o del %%~f))))
	if exist .\build\kernel\*.elf del .\build\kernel\*.elf
	if exist .\build\images\*.img del .\build\images\*.img

clean_mac:
#   Remove old build files (./src/build)
	rm -rf ./src/build
	
#   Remove new build files (./build)
	find ./build -path '*.o' -not -iname 'video_src.o' -not -iname 'img_src.o' -not -iname 'map-bitmap.o' -not -iname 'welcome_img.o' -not -iname 'game-menu.o' -exec rm {} \;
	rm -f ./build/kernel/*.elf
	rm -f ./build/images/*.img

clean_img:
	$(RMIMG)

clean_video:
	$(RMVIDEO)

clean_map:
	$(RMMAP)

clean_welcome:
	$(RMWELCOME)

clean_gamemenu:
	$(RMGAMEMENU)

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

$(BUILD_DIR)/%.o: $(OS_DIR)/mailbox/%.c
	$(CC) $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/core/%.c
	$(CC) $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/utils/%.c
	$(CC) $(GCCFLAGS) -c $< -o $@
	
$(BUILD_DIR)/%_asm.o: $(LIB_DIR)/utils/%.S
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
