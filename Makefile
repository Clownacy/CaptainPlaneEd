NAME	= CaptainPlaneEd
CXXFLAGS	= `sdl2-config --cflags` -O2 -s -std=c++14 -Wall -Wextra -pedantic -fno-ident
ifeq ($(OS),Windows_NT)
  CXXFLAGS	+= -static
endif

DEFINES	= 

ifeq ($(OS),Windows_NT)
  LIBS	= `sdl2-config --static-libs`
else
  LIBS	= `sdl2-config --libs`
endif

OBJDIR = ./build

OBJECTS = $(OBJDIR)/PlaneEd.cpp.o $(OBJDIR)/TxtRead.cpp.o $(OBJDIR)/PrjHndl.cpp.o $(OBJDIR)/Tile.cpp.o $(OBJDIR)/Graphics.cpp.o $(OBJDIR)/LevMap.cpp.o $(OBJDIR)/SelRect.cpp.o $(OBJDIR)/Resource.cpp.o $(OBJDIR)/Screen.cpp.o $(OBJDIR)/Project.cpp.o
ifeq ($(OS),Windows_NT)
OBJECTS += $(OBJDIR)/WinAPI.o
endif
OBJECTS += $(OBJDIR)/compression/KidDec.c.o $(OBJDIR)/compression/ReadPlain.c.o
OBJECTS += $(OBJDIR)/compression/FW_KENSC/comper.cc.o $(OBJDIR)/compression/FW_KENSC/enigma.cc.o $(OBJDIR)/compression/FW_KENSC/kosinski.cc.o $(OBJDIR)/compression/FW_KENSC/nemesis.cc.o $(OBJDIR)/compression/FW_KENSC/saxman.cc.o $(OBJDIR)/compression/FW_KENSC/rocket.cc.o

all: $(OBJECTS) $(NAME)

$(OBJDIR)/%.o: %
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(DEFINES) -c $^ -o $@

$(NAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(DEFINES) $^ $(LIBS) -o $@
