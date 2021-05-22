# Variables
DEBUG := 0
WINDOWS := 0

ifeq ($(OS),Windows_NT)
  WINDOWS := 1
endif

# Constants
NAME := CaptainPlaneEd

CXXFLAGS := $(shell pkg-config --cflags sdl2) -std=c++14 -Wall -Wextra -pedantic

ifeq ($(DEBUG),1)
  CXXFLAGS += -Og -ggdb3 -fsanitize=address
else
  CXXFLAGS += -O2 -s -fno-ident
endif

ifeq ($(WINDOWS),1)
  CXXFLAGS += -static
endif

ifeq ($(WINDOWS),1)
  LIBS := $(shell pkg-config --static --libs sdl2)
else
  LIBS := $(shell pkg-config --libs sdl2)
endif

OBJDIR := ./build

# Source files

# CaptainPlaneEd
OBJECTS := $(OBJDIR)/PlaneEd.cpp.o $(OBJDIR)/TxtRead.cpp.o $(OBJDIR)/PrjHndl.cpp.o $(OBJDIR)/Tile.cpp.o $(OBJDIR)/Graphics.cpp.o $(OBJDIR)/LevMap.cpp.o $(OBJDIR)/SelRect.cpp.o $(OBJDIR)/Resource.cpp.o $(OBJDIR)/Screen.cpp.o $(OBJDIR)/Project.cpp.o

# Windows stuff
ifeq ($(WINDOWS),1)
  OBJECTS += $(OBJDIR)/WinAPI.cpp.o
endif

# Misc. decompression
OBJECTS += $(OBJDIR)/compression/KidDec.c.o $(OBJDIR)/compression/ReadPlain.c.o

# FW-KENSC
OBJECTS += $(OBJDIR)/compression/FW_KENSC/comper.cc.o $(OBJDIR)/compression/FW_KENSC/enigma.cc.o $(OBJDIR)/compression/FW_KENSC/kosinski.cc.o $(OBJDIR)/compression/FW_KENSC/nemesis.cc.o $(OBJDIR)/compression/FW_KENSC/saxman.cc.o $(OBJDIR)/compression/FW_KENSC/rocket.cc.o

# Rules
all: $(OBJECTS) $(NAME)

$(OBJDIR)/%.o: %
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(NAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@
