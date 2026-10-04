#---------------------------------------------------------------------------------
# WiiBlox - Nintendo Wii Homebrew
#---------------------------------------------------------------------------------

.SUFFIXES:

ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment.")
endif

include $(DEVKITPPC)/wii_rules

#---------------------------------------------------------------------------------
# Project configuration
#---------------------------------------------------------------------------------

TARGET  := WiiBlox
BUILD   := build
SOURCES := source
INCLUDES := include

#---------------------------------------------------------------------------------
# Compiler flags
#---------------------------------------------------------------------------------

CFLAGS := -g -O2 -Wall -Wextra $(MACHDEP) $(INCLUDE)

LDFLAGS := -g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# Libraries
#---------------------------------------------------------------------------------

LIBS := -lwiiuse -lbte -lfat -logc -lm

LIBDIRS := $(PORTLIBS)

#---------------------------------------------------------------------------------
# Automatic source discovery
#---------------------------------------------------------------------------------

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT := $(CURDIR)/$(TARGET)

export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))

OFILES_SOURCES := $(CFILES:.c=.o)

# Use the PowerPC compiler driver for linking.
# Wii projects written in C must link through $(CC).
export LD := $(CC)

export OFILES := $(OFILES_SOURCES)

export HFILES :=

export INCLUDE := \
    $(foreach dir,$(INCLUDES),-iquote $(CURDIR)/$(dir)) \
    $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
    -I$(CURDIR)/$(BUILD) \
    -I$(LIBOGC_INC)

export LIBPATHS := \
    $(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
    -L$(LIBOGC_LIB)

.PHONY: all clean

all: $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo clean ...
	@rm -rf $(BUILD) $(OUTPUT).elf $(OUTPUT).dol $(OUTPUT).map

else

DEPENDS := $(OFILES:.o=.d)

$(OUTPUT).dol: $(OUTPUT).elf

$(OUTPUT).elf: $(OFILES)

$(OFILES_SOURCES): $(HFILES)

-include $(DEPENDS)

endif
