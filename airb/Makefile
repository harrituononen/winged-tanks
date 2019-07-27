# Airborne Mayhem Makefile

CC = g++
CFLAGS += -std=c++14

# Compile with a RELEASE value other than 0 for a release build
RELEASE ?= 0
ifeq ($(RELEASE), 0)
	CFLAGS += -DDEBUG -g
	TARGET = airb_debug
    OBJDIR = obj/debug
    DEPDIR = dep/debug
else
	CFLAGS += -DNDEBUG -O2
	TARGET = airb_release
    OBJDIR = obj/release
    DEPDIR = dep/release
endif

# Warnings
CFLAGS += -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wno-unused-parameter -Weffc++

# Directories
SRCDIR = src

# File extensions
SRCEXT = cpp
OBJEXT = obj
DEPEXT = dep

# Files
SRC = \
	$(wildcard $(SRCDIR)/*.$(SRCEXT)) \
	$(wildcard $(SRCDIR)/*/*.$(SRCEXT)) \
	$(wildcard $(SRCDIR)/*/*/*.$(SRCEXT))
OBJ = $(patsubst $(SRCDIR)/%.$(SRCEXT), $(OBJDIR)/%.$(OBJEXT), $(SRC))
DEP = $(patsubst $(SRCDIR)/%.$(SRCEXT), $(DEPDIR)/%.$(DEPEXT), $(SRC))

REQTODEP = $(patsubst $(SRCDIR)/%.$(SRCEXT), $(DEPDIR)/%.$(DEPEXT), $<)

# Dependencies
CFLAGS += -MMD -MP -MF $(REQTODEP)

# External libraries
CFLAGS += $(shell pkg-config --cflags glfw3 glew)
LFLAGS += $(shell pkg-config --libs glfw3 glew)
CFLAGS += -I/usr/local/include/gorilla/ -I/usr/local/include/freetype-gl
LFLAGS += -L/usr/local/lib/gorilla/ -L/usr/local/lib/freetype-gl/
LFLAGS += -lfreetype -lgorilla -lpthread -lopenal

# Phony targets
.PHONY : all clean

# Rules
all : $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LFLAGS)
clean :
	$(RM) $(OBJ) $(DEP) $(TARGET)
$(OBJDIR)/%.$(OBJEXT) : $(SRCDIR)/%.$(SRCEXT)
	$(shell mkdir -p $(dir $@))
	$(shell mkdir -p $(dir $(REQTODEP)))
	$(CC) -c $< -o $@ $(CFLAGS)

-include $(DEP)
