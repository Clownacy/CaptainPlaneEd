NAME	= CaptainPlaneEd
GCC	= g++
CFLAGS	= `sdl2-config --cflags` -O3 -s -std=c++14 -Wall -Wextra -fno-ident
DEFINES	= 
LIBS	= `sdl2-config --libs`

OBJDIR = ./build

OBJECTS = $(OBJDIR)/PlaneEd.o $(OBJDIR)/TxtRead.o $(OBJDIR)/PrjHndl.o $(OBJDIR)/Tile.o $(OBJDIR)/Graphics.o $(OBJDIR)/LevMap.o $(OBJDIR)/SelRect.o $(OBJDIR)/Resource.o $(OBJDIR)/Screen.o $(OBJDIR)/Project.o
ifeq ($(OS),Windows_NT)
OBJECTS += $(OBJDIR)/WinAPI.o
endif
OBJECTS += $(OBJDIR)/KidDec.o $(OBJDIR)/ReadPlain.o
OBJECTS += $(OBJDIR)/comper.o $(OBJDIR)/enigma.o $(OBJDIR)/kosinski.o $(OBJDIR)/nemesis.o $(OBJDIR)/saxman.o

all: objdirectory $(OBJECTS) $(NAME)

objdirectory:
	mkdir -p $(OBJDIR)

$(OBJDIR)/PlaneEd.o: PlaneEd.cpp
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/TxtRead.o: TxtRead.cpp
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/PrjHndl.o: PrjHndl.cpp
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/Tile.o: Tile.cpp
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/Graphics.o: Graphics.cpp
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/LevMap.o: LevMap.cpp
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/SelRect.o: SelRect.cpp
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/Resource.o: Resource.cpp
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/Screen.o: Screen.cpp
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/Project.o: Project.cpp
	$(GCC) $(CFLAGS) -c $^ -o $@

ifeq ($(OS),Windows_NT)
$(OBJDIR)/WinAPI.o: WinAPI.cpp
	$(GCC) $(CFLAGS) -c $^ -o $@
endif

$(OBJDIR)/KidDec.o: compression/KidDec.c
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/ReadPlain.o: compression/ReadPlain.c
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/comper.o: compression/FW_KENSC/comper.cc
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/enigma.o: compression/FW_KENSC/enigma.cc
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/kosinski.o: compression/FW_KENSC/kosinski.cc
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/nemesis.o: compression/FW_KENSC/nemesis.cc
	$(GCC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/saxman.o: compression/FW_KENSC/saxman.cc
	$(GCC) $(CFLAGS) -c $^ -o $@

$(NAME): $(OBJECTS)
	$(GCC) $(CFLAGS) $(DEFINES) $^ $(LIBS) -o $@
