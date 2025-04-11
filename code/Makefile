# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Source files
SRCS = codes/main.c codes/activities.c codes/iMan.c codes/bg.c codes/fg.c codes/controls.c codes/neonate.c codes/ping.c codes/prompt.c codes/alias.c codes/process.c codes/command_log.c codes/hop.c codes/reveal.c codes/proclore.c codes/seek.c codes/pipes_io.c	

# Object files (replace .c with .o in the SRCS list, maintaining paths)
OBJS = $(SRCS:.c=.o)

# Default target executable name
EXEC = a.out

# Default target
all: $(EXEC)

# Linking object files to create the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

# Compiling .c files to .o files
# This rule compiles each .c file into a corresponding .o file in the same directory as the .c file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJS) $(EXEC)

# Run the shell
run: $(EXEC)
	./$(EXEC)

# Phony targets
.PHONY: all clean run
