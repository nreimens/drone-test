HEX_FILE_START_OFFSET         ?= 0
VERBOSE 					  ?= 1

# source locations...
SRC_DIR   := ../src
NRFX_ROOT  := ../nrfx
BUILD_ROOT := .

# build output locations...
LISTING_DIRECTORY       = $(BUILD_ROOT)/lst
OBJECT_DIRECTORY        = $(BUILD_ROOT)/obj
OUTPUT_BINARY_DIRECTORY = $(BUILD_ROOT)/bin
OUTPUT_HEX_DIRECTORY    = $(BUILD_ROOT)/hex
OUTPUT_DFU_DIRECTORY    = $(BUILD_ROOT)/dfu

BUILD_DIRECTORIES := $(sort \
  $(OBJECT_DIRECTORY) \
  $(OUTPUT_BINARY_DIRECTORY) \
  $(LISTING_DIRECTORY) \
  $(OUTPUT_HEX_DIRECTORY) \
  $(OUTPUT_DFU_DIRECTORY) )

NRFX_SRC_FILES = $(NRFX_ROOT)/drivers/src/nrfx_systick.c \
				 $(NRFX_ROOT)/mdk/system_nrf52840.c	\

APP_SRC_FILES = $(SRC_DIR)/main.c \

# combine our source files into a single list
C_SOURCE_FILES = $(NRFX_SRC_FILES) $(APP_SRC_FILES)


#####################################
#
# Main include dirs
#
INC_PATHS += \
  -I$(SRC_DIR)/ \
  -I../config \
  -I../CMSIS \


#####################################
#
# nrfx include directories...
#
INC_PATHS += \
  -I$(NRFX_ROOT)/ \
  -I$(NRFX_ROOT)/drivers/include \
  -I$(NRFX_ROOT)/mdk \
  -I$(NRFX_ROOT)/hal \

#####################################
#
# Assembly files
#
ASM_SOURCE_FILES  += $(NRFX_ROOT)/mdk/gcc_startup_nrf52840.S


ifeq ($(OS),Windows_NT)
GNU_INSTALL_ROOT := C:/Program Files (x86)/gcc-arm
NRF_TOOLS_ROOT   := C:/Program Files (x86)/Nordic Semiconductor/nrf5x/bin
else
GNU_INSTALL_ROOT := /usr/local/gcc-arm
NRF_TOOLS_ROOT   := /usr/local/nRF5x
endif


GNU_PREFIX := arm-none-eabi

MK := mkdir
RM := rm -rf
CP := cp

#function for removing duplicates in a list
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

#echo suspend
ifeq ("$(VERBOSE)","1")
NO_ECHO := 
else
NO_ECHO := @
endif

# Toolchain commands
CC              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc'
AS              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as'
AR              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar' -r
LD              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ld'
NM              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm'
OBJDUMP         := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump'
OBJCOPY         := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy'
SIZE            := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-size'

NRFUTIL         := nrfutil
ifeq ($(OS),Windows_NT)
NRFJPROG        ?= "$(NRF_TOOLS_ROOT)/nrfjprog"
else
NRFJPROG        ?= $(NRF_TOOLS_ROOT)/nrfjprog/nrfjprog
endif

#
# some debug specific build flags
#
DBGFLAGS += \
  -DDEBUG_NRF \
  -DDEBUG \
  -O0 -g3 \
  -DMMD -ggdb 

# Optimization flags
OPT = -O0 -g3

#
# SDK CFLAGS
#
CFLAGS += $(OPT)
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
#CFLAGS += -DSOFTDEVICE_PRESENT
#CFLAGS += -DS$(SOFT_DEVICE)
#CFLAGS += -DNRF52
CFLAGS += -DNRF52840_XXAA
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums
CFLAGS += --std=gnu99

ifeq ($(BUILD_TYPE),Debug)
CFLAGS += $(DBGFLAGS)
endif


#
# assembler flags
#
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
#ASMFLAGS += -DSOFTDEVICE_PRESENT
#ASMFLAGS += -DS$(SOFT_DEVICE)
ASMFLAGS += -DNRF52840_XXAA
ASMFLAGS += -D__HEAP_SIZE=0

# Linker flags
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(APP_NAME).map
LDFLAGS += -mthumb -mabi=aapcs -L$(NRFX_ROOT)/mdk -Tnrf52840_xxaa.ld
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIBS += -lc -lnosys -lm

MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
C_PATHS = $(call remduplicates, $(dir $(C_SOURCE_FILES) ) )
C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILE_NAMES:.c=.o) )

ASM_SOURCE_FILE_NAMES = $(notdir $(ASM_SOURCE_FILES))
ASM_PATHS = $(call remduplicates, $(dir $(ASM_SOURCE_FILES) ))
ASM_DOT_O_FILES = $(patsubst %.s,%.o,$(filter %.s,$(ASM_SOURCE_FILE_NAMES))) $(patsubst %.S,%.o,$(filter %.S,$(ASM_SOURCE_FILE_NAMES)))
ASM_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASM_DOT_O_FILES) )

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

vpath %.c $(C_PATHS)
vpath %.s $(ASM_PATHS)
vpath %.S $(ASM_PATHS)

BUILD_FILENAME=app_test

LINK_OUTPUT=$(OUTPUT_BINARY_DIRECTORY)/$(BUILD_FILENAME).out
BIN_OUTPUT=$(OUTPUT_BINARY_DIRECTORY)/$(BUILD_FILENAME).bin
CRC_OUTPUT=$(OUTPUT_BINARY_DIRECTORY)/$(BUILD_FILENAME)_crc.bin
HEX_OUTPUT=$(OUTPUT_HEX_DIRECTORY)/$(BUILD_FILENAME).hex

# For debug
#$(info MAKELEVEL=$(MAKELEVEL))
#$(info MAKECMDGOALS=$(MAKECMDGOALS))

.PHONY: default
default: $(HEX_OUTPUT) echosize

# Create build directories
$(BUILD_DIRECTORIES):
	@$(MK) $@

# Any object requires that the build directories exist, but the build directories are an order-only prerequisite so that the modification date of the build directories is ignored
$(OBJECTS) : | $(BUILD_DIRECTORIES)

# create .out file from the object files (.o)
$(LINK_OUTPUT): $(OBJECTS)
	@echo Linking target: $(LINK_OUTPUT)
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(LINK_OUTPUT)
	$(NO_ECHO)$(CP) $(LINK_OUTPUT) $(OUTPUT_BINARY_DIRECTORY)/$(APP_NAME).out

# create .bin file from .out file
$(BIN_OUTPUT): $(LINK_OUTPUT)
	@echo Preparing: $(BIN_OUTPUT)
	@rm -f $(OUTPUT_BINARY_DIRECTORY)/*.bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(LINK_OUTPUT) $(BIN_OUTPUT)

# create .hex file from .bin file
$(HEX_OUTPUT): $(BIN_OUTPUT)
	@echo Preparing: $(HEX_OUTPUT)
	@rm -f $(OUTPUT_HEX_DIRECTORY)/*.hex
	$(NO_ECHO)$(OBJCOPY) -I binary -O ihex --change-addresses $(HEX_FILE_START_OFFSET) $(BIN_OUTPUT) $(HEX_OUTPUT)

#$(NO_ECHO)

# print size information about different sections of the executable
.PHONY: echosize
echosize: $(LINK_OUTPUT)
	-@echo ''
	$(NO_ECHO)$(SIZE) $(LINK_OUTPUT)
	-@echo ''

# Generic rule to build all remaining C files
$(OBJECT_DIRECTORY)/%.o: %.c
	@echo Compiling: $(notdir $<)
	$(NO_ECHO)$(CC) $(CFLAGS) $(INC_PATHS) -c -o $@ $<
	
# Assemble files
define assemble_file
	@echo Assembly file: $(notdir $<)
	$(NO_ECHO)$(CC) $(ASMFLAGS) $(INC_PATHS) -c -o $@ $<
endef

$(OBJECT_DIRECTORY)/%.o: %.s
	$(assemble_file)
$(OBJECT_DIRECTORY)/%.o: %.S
	$(assemble_file)
	
.PHONY: clean
clean:
	$(RM) $(BUILD_DIRECTORIES)


.PHONY: flash
flash:
	@if [ `ls $(OUTPUT_HEX_DIRECTORY)/*.hex | wc -l` -eq 0 ]; then echo No hex file in $(OUTPUT_HEX_DIRECTORY)!; exit 1; fi
	@if [ `ls $(OUTPUT_HEX_DIRECTORY)/*.hex | wc -l` -ne 1 ]; then echo Multiple hex files present in $(OUTPUT_HEX_DIRECTORY)!; exit 1; fi
	@$(eval FLASH_FILE=$(shell ls $(OUTPUT_HEX_DIRECTORY)/*.hex))
	@echo Flashing: $(notdir $(FLASH_FILE))
	$(NRFJPROG) --program $(FLASH_FILE) -f nrf52  --sectorerase
	$(NRFJPROG) --reset -f nrf52