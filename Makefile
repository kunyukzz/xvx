CC = clang
STD = -std=c99
NAME = xvx 

# Detect OS
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	PLATFORM_LIBS = -lGL -lm -ldl -lrt -lX11
	GLFW_LIB = -lglfw
else ifeq ($(OS),Windows_NT)
	PLATFORM_LIBS = -lopengl32 -lm -luser32 -lgdi32 -lkernel32
else
    $(error Unsupported OS)
endif

MODE ?= debug
DEBUG_FLAGS = -g -MD
RELEASE_FLAGS = -O3

ifeq ($(MODE),debug)
	DEFINES += -DDEBUG -DASSERT_ENABLE
	COMMONS = $(DEBUG_FLAGS)
else ifeq ($(MODE),release)
	DEFINES += -D_RELEASE
	COMMONS = $(RELEASE_FLAGS)
else
	$(error Unknown build mode: $(MODE))
endif

# GLFW stuff
# GLFW_DIR = src/vendor/glfw-3.4

# Add all subdirs in src to include path (recursive)
WARNINGS = -Wall -Wextra -Wno-c2x-extensions -Wpointer-arith -Wcast-align -Wconversion \
		   -Wstrict-aliasing -Wno-gnu-zero-variadic-macro-arguments
INCLUDES = -Isrc -Isrc/deps/glad 

# Final compiler flags
CFLAGS = $(STD) $(WARNINGS) $(INCLUDES) $(COMMONS) $(DEFINES)

# Source and object files
SRC = $(shell find src -name '*.c')
OBJ = $(SRC:%.c=obj/%.o)
DEP = $(OBJ:.o=.d)

TARGET = bin/$(NAME)
all: $(TARGET)

# Build
$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	@echo "Linking $@"
	@$(CC) -o $@ $(OBJ) $(GLFW_LIB) $(PLATFORM_LIBS)

# Rule for building object files in obj/ folder
obj/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	@echo "Cleaning..."
	@rm -rf obj bin/$(GAME_NAME)

# Clean All
clean-all:
	@echo "Cleaning all dependency..."
	@rm -rf obj bin

# Include dependency files
-include $(DEP)

.PHONY: all clean clean-all
