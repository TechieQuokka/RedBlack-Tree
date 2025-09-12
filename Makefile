CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
LDFLAGS = -lm

SRCDIR = .
OBJDIR = obj
BINDIR = bin

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/rbtree_test

LIBRARY = $(BINDIR)/librbtree.a
LIB_OBJECTS = $(OBJDIR)/rbtree.o

.PHONY: all clean test debug release library

all: $(TARGET)

library: $(LIBRARY)

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(LIBRARY): $(LIB_OBJECTS) | $(BINDIR)
	ar rcs $@ $(LIB_OBJECTS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

test: $(TARGET)
	@echo "Running Red-Black Tree tests..."
	@$(TARGET)

debug: CFLAGS += -DDEBUG -g
debug: $(TARGET)

release: CFLAGS += -O3 -DNDEBUG
release: clean $(TARGET)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

install: $(LIBRARY)
	@echo "Installing Red-Black Tree library..."
	cp rbtree.h /usr/local/include/ || echo "Could not install header (run as root)"
	cp $(LIBRARY) /usr/local/lib/ || echo "Could not install library (run as root)"

uninstall:
	rm -f /usr/local/include/rbtree.h
	rm -f /usr/local/lib/librbtree.a

help:
	@echo "Available targets:"
	@echo "  all      - Build test executable (default)"
	@echo "  library  - Build static library"
	@echo "  test     - Build and run tests"
	@echo "  debug    - Build with debug flags"
	@echo "  release  - Build optimized release"
	@echo "  clean    - Remove build files"
	@echo "  install  - Install library system-wide"
	@echo "  uninstall- Remove installed library"
	@echo "  help     - Show this help message"

# Dependencies
$(OBJDIR)/rbtree.o: rbtree.c rbtree.h
$(OBJDIR)/test.o: test.c rbtree.h