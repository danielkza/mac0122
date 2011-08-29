CC = gcc
CFLAGS = -O1 -Wall -Wextra
LDFLAGS =
EXECUTABLE = tr
SRCDIR = ./src
OBJDIR = ./build

SOURCES := $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c)))
OBJS := $(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))

VPATH = $(SRCDIR)

all: $(OBJDIR)/$(EXECUTABLE)

$(OBJDIR)/$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	-rm -f $(OBJDIR)/*.o $(EXECUTABLE)


