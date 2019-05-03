# You can change this

# Name of the project
PROJECT_NAME := SO1819

# The names of every executable; both in a list and individually
EXECUTABLE_LIST := ma sv cv ag

EXECUTABLE_MA := ma
EXECUTABLE_SV := sv
EXECUTABLE_CV := cv
EXECUTABLE_AG := ag

# Which objects to place in each executable.
# Objects are assumed to be located under the src/ directory,
# and are assumed to have no extensions (these are added automatically)
EXECUTABLE_MA_OBJECTS := common/commands common/fdb common/strings ma/main
EXECUTABLE_SV_OBJECTS := common/commands common/fdb common/strings sv/sv sv/main
EXECUTABLE_CV_OBJECTS := common/commands common/fdb common/strings cv/main
EXECUTABLE_AG_OBJECTS := common/commands common/fdb common/strings ag/main

# Compiler to use
GCC := gcc
# Compiler flags to use in all cases
GCC_FLAGS := -std=c11 -Wall -Wextra -pedantic
# Compiler flags to use for production builds
GCC_PRODUCTION_FLAGS := -O2
# Compiler flags to use for debug builds
GCC_DEBUG_FLAGS := -O0 -ggdb

# Debugger to use
GDB := gdb
# Flags to use when running the debugger
GDB_FLAGS := -return-child-result -x gdbinit

# Which strace executable should we run
STRACE := strace
# Flags to use when running strace
STRACE_FLAGS := 

# Don't change from here


## Various flag-dependant environment variables

### Output locations
# Attention for $(OBJ_DIR): /production or /debug are always suffixed, depending on the value of $(debug)
OUT_DIR := bin
OBJ_DIR := obj


## Flags

### Are we building the debug binaries?
debug ?= true

ifeq ($(debug), true)
	GCC_FLAGS += $(GCC_DEBUG_FLAGS)
    OBJ_DIR := $(addsuffix /debug, $(OBJ_DIR))
    OUT_DIR := $(addsuffix /debug, $(OUT_DIR))
else
	GCC_FLAGS += $(GCC_PRODUCTION_FLAGS)
    OBJ_DIR := $(addsuffix /production, $(OBJ_DIR))
    OUT_DIR := $(addsuffix /production, $(OUT_DIR))
endif

### Which object are we building?
bin ?= $(firstword $(MAKECMDGOALS))

ifeq ($(bin),)
bin := all
endif

bin := $(addprefix $(OUT_DIR)/, $(bin))

### Name of the zip file?
zipname ?= $(PROJECT_NAME).zip


## Flag-independant environment variables

### Build the names of the modules that go in the ma executable
EXECUTABLE_MA_OBJECTS := $(addprefix $(OBJ_DIR)/, $(EXECUTABLE_MA_OBJECTS))
EXECUTABLE_MA_OBJECTS := $(addsuffix .o, $(EXECUTABLE_MA_OBJECTS))

### Build the names of the modules that go in the sv executable
EXECUTABLE_SV_OBJECTS := $(addprefix $(OBJ_DIR)/, $(EXECUTABLE_SV_OBJECTS))
EXECUTABLE_SV_OBJECTS := $(addsuffix .o, $(EXECUTABLE_SV_OBJECTS))

### Build the names of the modules that go in the cv executable
EXECUTABLE_CV_OBJECTS := $(addprefix $(OBJ_DIR)/, $(EXECUTABLE_CV_OBJECTS))
EXECUTABLE_CV_OBJECTS := $(addsuffix .o, $(EXECUTABLE_CV_OBJECTS))

### Build the names of the modules that go in the ag executable
EXECUTABLE_AG_OBJECTS := $(addprefix $(OBJ_DIR)/, $(EXECUTABLE_AG_OBJECTS))
EXECUTABLE_AG_OBJECTS := $(addsuffix .o, $(EXECUTABLE_AG_OBJECTS))

### Build a list of all project source files
ALL_FILES = src gdbinit Makefile Doxyfile README.md


## Targets

### Build Targets

#### Executable targets

##### Default Target: Build all the executables
all: $(EXECUTABLE_LIST)

##### Target: Build the ma executable
$(OUT_DIR)/$(EXECUTABLE_MA): $(EXECUTABLE_MA_OBJECTS) $(OBJ_DIR)/ma/main.o
	$(GCC) $(GCC_FLAGS) -o $@ $^

##### Target: Build the sv executable
$(OUT_DIR)/$(EXECUTABLE_SV): $(EXECUTABLE_SV_OBJECTS) $(OBJ_DIR)/sv/main.o
	$(GCC) $(GCC_FLAGS) -o $@ $^

##### Target: Build the cv executable
$(OUT_DIR)/$(EXECUTABLE_CV): $(EXECUTABLE_CV_OBJECTS) $(OBJ_DIR)/cv/main.o
	$(GCC) $(GCC_FLAGS) -o $@ $^

##### Target: Build the ag executable
$(OUT_DIR)/$(EXECUTABLE_AG): $(EXECUTABLE_AG_OBJECTS) $(OBJ_DIR)/ag/main.o
	$(GCC) $(GCC_FLAGS) -o $@ $^

##### Target: Alias to just build the binaries regardless of their names
ma: $(OUT_DIR)/$(EXECUTABLE_MA)
sv: $(OUT_DIR)/$(EXECUTABLE_SV)
cv: $(OUT_DIR)/$(EXECUTABLE_CV)
ag: $(OUT_DIR)/$(EXECUTABLE_AG)


#### Object targets

##### Target: any object under src
$(OBJ_DIR)/%.o: src/%.c setup
	$(GCC) $(GCC_FLAGS) -o $@ -c $<


### Setup & Cleanup targets

#### Sets up the folder structure for the project
setup:
	mkdir -p bin/production
	mkdir -p bin/debug
	mkdir -p doc
	mkdir -p obj/production/common
	mkdir -p obj/production/ma
	mkdir -p obj/production/sv
	mkdir -p obj/production/cv
	mkdir -p obj/production/ag
	mkdir -p obj/debug/common
	mkdir -p obj/debug/ma
	mkdir -p obj/debug/sv
	mkdir -p obj/debug/cv
	mkdir -p obj/debug/ag
	touch setup

#### Cleans up all binaries, objects, documentation, created zip files, and marks the project as not setup
clean:
#	Apaga os diretórios, conteúdo e ficheiros chamados: setup, bin, doc, obj, install, $(zipname)
	rm -rf setup bin doc obj $(zipname)


### Runnable targets

#### Runs the specified binary normally
run: $(bin)
	./$(bin)

#### Execute the specified binary through gdb
run-gdb: $(bin)
	$(GDB) $(GDB_FLAGS) $(bin)

#### Execute the program through strace
run-strace: $(bin)
	$(STRACE) $(STRACE_FLAGS) $(bin)

### Other targets

#### Builds the documentation for the project
doc:
	doxygen

#### Builds a zip file containing all files necessary to move this project to another location
zip:
	zip -r -9 $(zipname) $(ALL_FILES)


#### Phony targets are always rebuilt
.PHONY: clean doc zip
