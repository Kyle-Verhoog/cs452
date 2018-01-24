GNU_COWAN = /u/wbcowan/gnuarm-4.0.2

ARM_XCC = $(GNU_COWAN)/arm-elf/bin/gcc
ARM_AS  = $(GNU_COWAN)/arm-elf/bin/as
ARM_LD  = $(GNU_COWAN)/arm-elf/bin/ld

X86_XCC = /usr/bin/gcc
X86_AS  = /usr/bin/as
X86_LD  = /usr/bin/gcc

XCC = $(ARM_XCC)
AS  = $(ARM_AS)
LD  = $(ARM_LD)
CP     = /bin/cp
ASTYLE = astyle

ARM_DIR ?= /u/cs452/tftp/ARM
BUILD_DIR ?= build
CONF_DIR  ?= conf
DOCS_DIR  ?= docs
TEST_DIR  ?= src/test

SRC_DIRS ?= src include
MD5_OUT  ?= $(BUILD_DIR)/md5_hashes.txt

TARGET     ?= a1
TARGET_ELF ?= $(TARGET).elf
TARGET_MAP ?= $(BUILD_DIR)/$(TARGET).map
TEST_EXEC  ?= tests

LINKER_SCRIPT ?= $(CONF_DIR)/orex.ld

SRC_IGNORE := $(wildcard src/test/*/*.c src/test/*.c)
$(info $(SRC_IGNORE))
SRCS := $(shell find $(SRC_DIRS) -name *.c)
SRCS := $(filter-out $(SRC_IGNORE), $(SRCS))
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

TEST_IGNORE := $(wildcard src/kernel/* src/user/* src/bwio/*)
TEST_SRCS := $(shell find $(TEST_DIR) -name *.c) $(filter-out $(TEST_IGNORE), $(SRCS))
#$(info $(TEST_SRCS))
TEST_OBJS := $(TEST_SRCS:%=$(BUILD_DIR)/%.o)
TEST_DEPS := $(TEST_OBJS:.o=.d)
# $(info $(TEST_OBJS))

INC_DIRS  := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

ARM_CFLAGS   = -c -fPIC -Wall -I. $(INC_FLAGS) -mcpu=arm920t -msoft-float -MMD -MP $(DFLAGS)
ARM_LDFLAGS  = -init main -Map $(TARGET_MAP) -N -T $(LINKER_SCRIPT) -L$(GNU_COWAN)/lib/gcc/arm-elf/4.0.2 -L./lib
TEST_CFLAGS  = -Wall -I. $(INC_FLAGS)
TEST_LDFLAGS =
CFLAGS  = $(ARM_CFLAGS)
LDFLAGS = $(ARM_LDFLAGS)


all: $(BUILD_DIR)/$(TARGET_ELF)

test: XCC = $(X86_XCC)
test: AS  = $(X86_AS)
test: LD  = $(X86_LD)
test: CFLAGS = $(TEST_CFLAGS)
test: LDFLAGS = $(TEST_LDFLAGS)
test: $(BUILD_DIR)/$(TEST_EXEC)
	./$(BUILD_DIR)/$(TEST_EXEC)

$(BUILD_DIR)/$(TEST_EXEC): $(TEST_OBJS)
	@$(LD) $(LDFLAGS) -o $@ $(TEST_OBJS)

$(BUILD_DIR)/$(TARGET_ELF): $(OBJS)
	@$(LD) $(LDFLAGS) -o $@ $(OBJS) -lgcc

$(BUILD_DIR)/%.o: %.s
	@$(MKDIR_P) $(dir $@)
	@$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.c.s: %.c
	@$(MKDIR_P) $(dir $@)
	@$(XCC) -S $(CFLAGS) $< -o $@

.PRECIOUS: $(BUILD_DIR)/%.c.s


# Hashes
hashes:
	$(MKDIR_P) $(BUILD_DIR)
	$(shell find -regex ".*\.[c|h]" -type f -exec md5sum '{}' \; > $(MD5_OUT))

# Docs
docs: $(addsuffix .pdf, $(basename $(wildcard $(DOCS_DIR)/*.md)))

$(DOCS_DIR)/%.pdf: $(DOCS_DIR)/%.md
	$(PANDOC) $(PANDOC_FLAGS) $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)

copy:
	$(MAKE) clean && $(MAKE) && $(CP) $(BUILD_DIR)/$(TARGET_ELF) $(ARM_DIR)/$(USER)

format:
	$(ASTYLE) --options=.astylerc "src/*.c"  "include/*.h"

.PHONY: clean hashes docs test format

-include $(DEPS)
# test: -include $(TEST_DEPS)

MKDIR_P ?= mkdir -p
PANDOC = pandoc
PANDOC_FLAGS=--metadata date="`date +'%b %d, %Y'`"
