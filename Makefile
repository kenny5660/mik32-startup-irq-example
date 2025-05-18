# Path you your toolchain installation, leave empty if already in system PATH
RISCV_TOOLCHAIN_ROOT ?=
MIK32_UPLOADER_DIR ?=
OPENOCD_PATH ?= /usr/bin/openocd

BUILD_DIR ?= build



HEX = $(BUILD_DIR)/main.hex
ELF = $(BUILD_DIR)/main.elf
SRC_DIR = src/
INC_DIR = inc/

# Toolchain
CC = $(RISCV_TOOLCHAIN_ROOT)riscv-none-elf-gcc
DB = $(RISCV_TOOLCHAIN_ROOT)riscv-none-elf-gdb
OBJCOPY = $(RISCV_TOOLCHAIN_ROOT)riscv-none-elf-objcopy

# Project sources
SRC_FILES = $(wildcard $(SRC_DIR)*.c) $(wildcard $(SRC_DIR)*/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)*.s) $(wildcard $(SRC_DIR)*/*.s)
LD_SCRIPT ?= $(SRC_DIR)/eeprom.ld

# Project includes
INCLUDES   = -I$(INC_DIR)

# Compiler Flags
CFLAGS  = -g -Os -Wall -ffunction-sections
CFLAGS += -march=rv32imc_zicsr_zifencei -mabi=ilp32 -mcmodel=medlow  
CFLAGS += $(INCLUDES)

# Linker Flags
LFLAGS = -Wl,--gc-sections -Wl,-T$(LD_SCRIPT) -nostartfiles

CXX_OBJS = $(SRC_FILES:.c=.o)
ASM_OBJS = $(ASM_FILES:.s=.o)
ALL_OBJS = $(ASM_OBJS) $(CXX_OBJS)



.PHONY: clean

all: $(BUILD_DIR) $(HEX) 


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile
$(CXX_OBJS): %.o: %.c
$(ASM_OBJS): %.o: %.s
$(ALL_OBJS):
	@echo "[CC] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link
$(ELF): $(ALL_OBJS)
	@echo "[LD] $@"
	@$(CC) $(CFLAGS) $(LFLAGS) $(ALL_OBJS) -o $@

$(HEX): $(ELF)
	@$(OBJCOPY) -O ihex $(ELF) $(HEX)

# Clean
clean:
	rm -f $(ALL_OBJS) $(TARGET)

flash:
	python3 $(MIK32_UPLOADER_DIR)/mik32_upload.py $(HEX) --run-openocd \
	--openocd-exec $(OPENOCD_DIR) \
	--openocd-target $(MIK32_UPLOADER_DIR)/openocd-scripts/target/mik32.cfg \
	--openocd-interface $(MIK32_UPLOADER_DIR)/openocd-scripts/interface/ftdi/mikron-link.cfg \
	--adapter-speed 500 --mcu-type MIK32V2