
XCC = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/gcc
AS  = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/as
LD  = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/ld

BUILD_DIR ?= ./build
CONF_DIR  ?= ./conf
DOCS_DIR  ?= ./docs

SRC_DIRS ?= src include
MD5_OUT ?= $(BUILD_DIR)/md5_hashes.txt

TARGET ?= a1
TARGET_ELF ?= $(TARGET).elf
TARGET_MAP ?= $(BUILD_DIR)/$(TARGET).map

LINKER_SCRIPT ?= $(CONF_DIR)/orex.ld

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS = -c -fPIC -Wall -I. $(INC_FLAGS) -mcpu=arm920t -msoft-float -MMD -MP
LDFLAGS = -init main -Map $(TARGET_MAP) -N -T $(LINKER_SCRIPT) -L/u/wbcowan/gnuarm-4.0.2/lib/gcc/arm-elf/4.0.2 -L./lib

all: $(BUILD_DIR)/$(TARGET_ELF) 

$(BUILD_DIR)/$(TARGET_ELF): $(OBJS)
		$(LD) $(LDFLAGS) -o $@ $(OBJS) -lgcc

$(BUILD_DIR)/%.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.c.s: %.c
	$(MKDIR_P) $(dir $@)
	$(XCC) -S $(CFLAGS) $< -o $@

.PRECIOUS: $(BUILD_DIR)/%.c.s

.PHONY: clean hashes docs

hashes:
	$(MKDIR_P) $(BUILD_DIR)
	$(shell find -regex ".*\.[c|h]" -type f -exec md5sum '{}' \; > $(MD5_OUT))

docs: $(addsuffix .pdf, $(basename $(wildcard $(DOCS_DIR)/*.md)))

$(DOCS_DIR)/%.pdf: $(DOCS_DIR)/%.md
	$(PANDOC) $(PANDOC_FLAGS) $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
PANDOC = pandoc
PANDOC_FLAGS=--metadata date="`date +'%b %d, %Y'`"
