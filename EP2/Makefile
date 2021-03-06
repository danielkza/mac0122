CC = gcc
CFLAGS = -O2 -Wall -Wextra -ansi -pedantic
LDFLAGS =

DOXYGEN:=$(shell which doxygen 2>/dev/null)

EXECUTABLE = matrix_regions
SRCDIR = ./src
OBJDIR = ./build
DOCDIR = ./docs

SOURCES := $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c)))
OBJS := $(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))

VPATH = $(SRCDIR)

$(OBJDIR)/$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: docs
docs:
ifeq ($(DOXYGEN),)
	@echo "* You don't have doxygen installed! Please install it if you want"
	@echo "* to generate the documentation files."
else
	cd $(SRCDIR); DOXYGEN_OUTPUT_DIR=../$(DOCDIR) doxygen ../$(DOCDIR)/Doxyfile
endif

.PHONY: clean
clean:
	-rm -f $(OBJDIR)/*.o $(OBJDIR)/*.exe $(EXECUTABLE)

all: $(OBJDIR)/$(EXECUTABLE) docs

