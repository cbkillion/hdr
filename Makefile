TARGET = packet_radio

# Project directory structure
SRCDIR = src
INCDIR = inc
BLDDIR = bld

# Find source files located in SRCDIR defined above
SRC  = $(wildcard $(SRCDIR)/*)

# Convert the source file names into the object file names
OBJT = $(addsuffix .o, $(basename $(SRC)))
OBJ = $(OBJT:$(SRCDIR)/%=$(BLDDIR)/%)

INCLUDE += -I./$(INCDIR)

# Define the linker script and chip
LD_SCRIPT = STM32F042K6Tx.ld
MCU_SPEC  = cortex-m0
OPTIM = -O0

# Define the toolchain
TOOLCHAIN = /usr
CC = $(TOOLCHAIN)/bin/arm-none-eabi-gcc
AS = $(TOOLCHAIN)/bin/arm-none-eabi-as
LD = $(TOOLCHAIN)/bin/arm-none-eabi-ld
OC = $(TOOLCHAIN)/bin/arm-none-eabi-objcopy
OD = $(TOOLCHAIN)/bin/arm-none-eabi-objdump
OS = $(TOOLCHAIN)/bin/arm-none-eabi-size

# Assembly directives
ASFLAGS += -c
ASFLAGS += $(OPTIM)
ASFLAGS += -mcpu=$(MCU_SPEC)
ASFLAGS += -mthumb
ASFLAGS += -Wall
ASFLAGS += -fmessage-length=0

# C compilation directives
CFLAGS += -mcpu=$(MCU_SPEC) -std=gnu11 -O0 -ffunction-sections -fdata-sections -Wall --specs=nano.specs -mfloat-abi=soft -mthumb
CFLAGS += -fmessage-length=0 $(OPTIM)

# Linker directives.
LFLAGS += -mcpu=$(MCU_SPEC) --specs=nosys.specs -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
LFLAGS += -nostdlib $(OPTIM)
# LFLAGS += -lgcc
LFLAGS += -T$(LD_SCRIPT)

.PHONY: all clean list-src flash
all: list-src $(BLDDIR)/$(TARGET).bin

$(BLDDIR)/$(TARGET).bin : $(BLDDIR)/$(TARGET).elf | $(BLDDIR)
	@echo -e '\e[1mCreating the binary:\e[0m $@'
	@$(OC) -O binary $< $@

$(BLDDIR)/$(TARGET).elf : $(OBJ)
	@echo -e '\e[1mCreating the ELF:\e[0m $@'
	@$(CC) $^ $(LFLAGS) -Wl,-Map,$(patsubst %.elf,%.map,$@) -o $@
	@$(OD) -DhS $@ > $(patsubst %.elf,%.list,$@)

	@echo
	@echo -e '\e[3mSection sizes:\e[0m'
	@$(OS) $@
	@echo
	@rm $(BLDDIR)/*.o

$(BLDDIR)/%.o : $(SRCDIR)/%.s | $(BLDDIR)
	@echo -e '\e[1mCompiling file:\e[0m $@'
	@$(CC) -x assembler-with-cpp $(ASFLAGS) $< -o $@

$(BLDDIR)/%.o : $(SRCDIR)/%.c | $(BLDDIR)
	@echo -e '\e[1mCompiling file:\e[0m $@'
	@$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

$(BLDDIR) :
	@echo -e '\e[1mCreating the build directory\e[0m'
	@mkdir -p $@

clean :
	@echo -e '\e[3mRemoving the build directory\e[0m'
	@rm -rf $(BLDDIR)

list-src :
	@echo
	@echo -e '\e[3mSources:\e[0m'
	@echo '    $(SRC)'
	@echo
	@echo -e '\e[3mObjects:\e[0m'
	@echo '    $(OBJ)'
	@echo

flash : $(BLDDIR)/$(TARGET).bin
	@echo -e '\e[1mUploading firmware:\e[0m $<'
	@echo
	dfu-util -a 0 -s 0x08000000:leave -D $(BLDDIR)/$(TARGET).bin