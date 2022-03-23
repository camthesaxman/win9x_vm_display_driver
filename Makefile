VERBOSE ?= 0
ifeq ($(VERBOSE),0)
  QUIET := @
endif

DEBUG ?= 1

WINE := wine

# Path to the ml.exe and link16.exe programs
MASM_PATH := ~/.wine/drive_c/masm32/bin

# Watcom include paths
export INCLUDE=$(WATCOM)/h/win

# Path to library files.
export LIB := lib

# Tools and options
CC         := wcc
ASM        := $(WINE) $(MASM_PATH)/ml.exe
LINK       := $(WINE) $(MASM_PATH)/link16.exe
ASM_FLAGS  := -c -Cp -nologo -Iinclude
LINK_FLAGS := /NOE/NOD/MAP/LI/AL:16
LINK_LIBS  := LIBW.LIB DIBENG.LIB
CFLAGS     := -3 -ml -zq -s -i=include -nd=_DATA

ifeq ($(DEBUG),1)
  ASM_FLAGS += -DDEBUG=1
  CFLAGS    += -dDEBUG=1
endif

# Files
DRIVERNAME := virtdisp
DRV_FILE := $(DRIVERNAME).drv
DEF_FILE := $(DRIVERNAME).def
MAP_FILE := $(DRIVERNAME).map
INF_FILE := $(DRIVERNAME).inf
OBJ_FILES := init.obj display.obj dibstubs.obj bga.obj debug.obj
FLOPPY_IMG := install.img

# Link driver
$(DRV_FILE): $(OBJ_FILES) $(DEF_FILE)
	@echo "LINK $@"
	$(QUIET) $(RM) $@
	$(QUIET) $(LINK) $(LINK_FLAGS) $(OBJ_FILES),$@,$(MAP_FILE),$(LINK_LIBS),$(DEF_FILE)

floppy: $(FLOPPY_IMG)

# Create an installation floppy disk image
$(FLOPPY_IMG): $(DRV_FILE) $(INF_FILE)
	@echo "Creating setup disk $@"
	$(QUIET) dd if=/dev/zero of=$@ bs=512 count=2880
	$(QUIET) mformat -i $@
	$(QUIET) mcopy -i $@ $^ ::

# Assemble object file
%.obj: %.asm
	@echo "ASM $@"
	$(QUIET) $(ASM) $(ASM_FLAGS) -Fo $@ -Fl$@.lst $<

# Compile object file
%.obj: %.c
	@echo "CC $@"
	$(QUIET) $(CC) $(CFLAGS) $< -fo=$@

clean:
	$(RM) *.obj *.map *.drv *.lnk *.img *.lst *.sym
