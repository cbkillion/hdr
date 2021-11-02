TARGET = packet_radio

# Project directory structure
SRCDIR = src
INCDIR = inc
BLDDIR = build

# Find source files located in SRCDIR defined above
SRC  = $(shell find $(SRCDIR) -name *.c -or -name *.s)
OBJ = $(SRC:$(SRCDIR)/%=$(BLDDIR)/%.o)
DEP = $(OBJ:.o=.d)

INCLUDE += -I./$(INCDIR)

# Define the linker script and chip
LD_SCRIPT = STM32F042K6Tx.ld
MCU_SPEC  = cortex-m0
OPTIM = -O3

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
CFLAGS += -mcpu=$(MCU_SPEC) -std=gnu11 --specs=nano.specs -mfloat-abi=soft -mthumb
CFLAGS += $(OPTIM)

# Linker directives.
LFLAGS += -mcpu=$(MCU_SPEC) --specs=nosys.specs -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
LFLAGS += -nostdlib
LFLAGS += -T$(LD_SCRIPT)

.PHONY: all clean list-src flash size

all: $(BLDDIR)/$(TARGET).bin size

$(BLDDIR)/$(TARGET).bin : $(BLDDIR)/$(TARGET).elf | $(BLDDIR)
	@$(OC) -O binary $< $@

$(BLDDIR)/$(TARGET).elf : $(OBJ)
	@$(CC) $^ $(LFLAGS) -Wl,-Map,$(patsubst %.elf,%.map,$@) -o $@
	@$(OD) -DhS $@ > $(patsubst %.elf,%.list,$@)
	@rm $(BLDDIR)/*.o

# Assembly file compilation
$(BLDDIR)/%.s.o : $(SRCDIR)/%.s | $(BLDDIR)
	@echo -e '\e[1mCompiling file:\e[0m $(basename $(@F))'
	@$(CC) -x assembler-with-cpp $(ASFLAGS) $< -o $@

# C file compilation
$(BLDDIR)/%.c.o : $(SRCDIR)/%.c | $(BLDDIR)
	@echo -e '\e[1mCompiling file:\e[0m $(basename $(@F))'
	@$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

# Create the build directory
$(BLDDIR) :
	@mkdir -p $@

clean :
	@rm -rf $(BLDDIR)

size : $(BLDDIR)/$(TARGET).elf
	@echo
	@echo -e '\e[3mSection sizes:\e[0m'
	@$(OS) $<

list-src :
	@echo -e '\e[3mSources:\e[0m'
	@echo '    $(SRC)'
	@echo -e '\e[3mObjects:\e[0m'
	@echo '    $(OBJ)'
	@echo

flash : $(BLDDIR)/$(TARGET).bin
	@echo -e '\e[1mUploading firmware:\e[0m $<'
	@echo
	dfu-util -a 0 -s 0x08000000:leave -D $(BLDDIR)/$(TARGET).bin