# Compiler flags go here.
CFLAGS = -g -Wall -std=c99

# Linker flags go here.
LDFLAGS = -lm -lwiringPi

# List of sources
SRC = main.c  gecko_bglib.c uart.c

# Program executable file name.
TARGET = run

# Top-level rule, to compile everything.
all: $(TARGET)

# Define all object files.
OBJ = $(SRC:.c=.o)

# Rule to link the program
$(TARGET): $(OBJ)
	gcc $(LDFLAGS) $^ -o $@

# Compile: create object files from C source files.
%.o : %.c
	gcc $(CFLAGS) -c  $< -o $@ 

# Rule for cleaning re-compilable files.
clean:
	rm -f $(TARGET) $(OBJ)

rebuild:	clean all

.PHONY: all rebuild clean