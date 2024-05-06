# Compiler and flags
CC = aarch64-none-elf-gcc
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib

# Directories
SRC_DIR = ./src
BUILD_DIR = $(SRC_DIR)/build
KERNEL_DIR = $(SRC_DIR)/os/kernel
LIB_DIR = $(SRC_DIR)/lib
IMAGE_DIR = $(SRC_DIR)/img
VIDEO_DIR = $(SRC_DIR)/video


# System and library files
KERNEL_CFILES = $(wildcard $(KERNEL_DIR)/*.c)
CORE_LIB_CFILES = $(wildcard $(LIB_DIR)/core/*.c)
MBOX_LIB_CFILES = $(wildcard $(LIB_DIR)/mailbox/*.c)
UTILS_LIB_CFILES = $(wildcard $(LIB_DIR)/utils/*.c)

# Media Processing files
IMAGE_CFILES = $(wildcard $(IMAGE_DIR)/*.c)
VIDEO_CFILES = $(wildcard $(VIDEO_DIR)/*.c)

# Game Engine files
GENGINE_LIB_CFILES = $(wildcard $(LIB_DIR)/games/engine/*.c)
UNROB_GAME_LIB_CFILES = $(wildcard $(LIB_DIR)/games/unrob/*.c)

# System files
CFILES = $(KERNEL_CFILES) $(CORE_LIB_CFILES) $(MBOX_LIB_CFILES) $(UTILS_LIB_CFILES) $(IMAGE_CFILES) $(VIDEO_CFILES) $(GENGINE_LIB_CFILES) $(UNROB_GAME_LIB_CFILES)
OFILES = $(addprefix $(BUILD_DIR)/, $(notdir $(CFILES:%.c=%.o)))

# print CFILES and OFILES
$(info CFILES = $(CFILES))
$(info OFILES = $(OFILES))

# Targets
UART = uart0
TARGET = kernel8.img
QEMU = qemu-system-aarch64

#----------------------------------------
# Main Rules
#----------------------------------------

all: mk_dirs clean build $(TARGET) run0

mac: mk_dirs_mac clean_mac build $(TARGET) run0_mac

$(TARGET): $(BUILD_DIR)/boot.o $(BUILD_DIR)/uart.o $(OFILES)
	aarch64-none-elf-ld -nostdlib $^ -T $(KERNEL_DIR)/link.ld -o $(BUILD_DIR)/kernel/kernel8.elf
	aarch64-none-elf-objcopy -O binary $(BUILD_DIR)/kernel/kernel8.elf $(BUILD_DIR)/images/$(TARGET)

#----------------------------------------
# Utility Rules 
#----------------------------------------

mk_dirs:
	if not exist .\src\build mkdir .\src\build
	if not exist .\src\build\kernel mkdir .\src\build\kernel
	if not exist .\src\build\images mkdir .\src\build\images

mk_dirs_mac:
	if [ ! -d ./src/build ]; then mkdir ./src/build; fi
	if [ ! -d ./src/build/kernel ]; then mkdir ./src/build/kernel; fi
	if [ ! -d ./src/build/images ]; then mkdir ./src/build/images; fi

clean:
	if exist .\src\build\*.o del .\src\build\*.o
	if exist .\src\build\kernel\*.elf del .\src\build\kernel\*.elf
	if exist .\src\build\images\*.img del .\src\build\images\*.img

clean_mac:
	rm -f .\src\build\*.o
	rm -f .\src\build\kernel\*.elf
	rm -f .\src\build\images\*.img

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
