CC = gcc
CFLAGS = -O1 -Wall
LDFLAGS =
EXECUTABLE = tr
SRCDIR = ./src
OBJDIR = ../build

VPATH=$(SRCDIR)

SOURCES := $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c)))
OBJS := $(SOURCES:.c=.o)

all: $(OBJDIR)/$(EXECUTABLE)

$(OBJDIR)/$(EXECUTABLE): $(addprefix $(OBJDIR)/, $(OBJS))
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/$(OBJS): $(SOURCES)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	\rm *.o $(EXECUTABLE)

.PHONY: show
show:
	echo $(SOURCES)
	echo $(OBJS)


