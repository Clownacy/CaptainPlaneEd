NAME	= CaptainPlaneEd
GCC	= g++
CXXFLAGS	= `sdl2-config --cflags` -O3 -s -static -std=c++14 -Wall -Wextra -fno-ident
DEFINES	= 
LIBS	= `sdl2-config --static-libs`

OBJDIR = ./build

OBJECTS = $(OBJDIR)/PlaneEd.o $(OBJDIR)/TxtRead.o $(OBJDIR)/PrjHndl.o $(OBJDIR)/Tile.o $(OBJDIR)/Graphics.o $(OBJDIR)/LevMap.o $(OBJDIR)/SelRect.o $(OBJDIR)/Resource.o $(OBJDIR)/Screen.o $(OBJDIR)/Project.o
ifeq ($(OS),Windows_NT)
OBJECTS += $(OBJDIR)/WinAPI.o
endif
OBJECTS += $(OBJDIR)/KidDec.o $(OBJDIR)/ReadPlain.o
OBJECTS += $(OBJDIR)/comper.o $(OBJDIR)/enigma.o $(OBJDIR)/kosinski.o $(OBJDIR)/nemesis.o $(OBJDIR)/saxman.o $(OBJDIR)/rocket.o

all: objdirectory $(OBJECTS) $(NAME)

objdirectory:
	mkdir -p $(OBJDIR)

$(OBJDIR)/PlaneEd.o: PlaneEd.cpp
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/TxtRead.o: TxtRead.cpp
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/PrjHndl.o: PrjHndl.cpp
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/Tile.o: Tile.cpp
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/Graphics.o: Graphics.cpp
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/LevMap.o: LevMap.cpp
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/SelRect.o: SelRect.cpp
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/Resource.o: Resource.cpp
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/Screen.o: Screen.cpp
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/Project.o: Project.cpp
	$(GCC) $(CXXFLAGS) -c $^ -o $@

ifeq ($(OS),Windows_NT)
$(OBJDIR)/WinAPI.o: WinAPI.cpp
	$(GCC) $(CXXFLAGS) -c $^ -o $@
endif

$(OBJDIR)/KidDec.o: compression/KidDec.c
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/ReadPlain.o: compression/ReadPlain.c
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/comper.o: compression/FW_KENSC/comper.cc
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/enigma.o: compression/FW_KENSC/enigma.cc
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/kosinski.o: compression/FW_KENSC/kosinski.cc
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/nemesis.o: compression/FW_KENSC/nemesis.cc
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/saxman.o: compression/FW_KENSC/saxman.cc
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(OBJDIR)/rocket.o: compression/FW_KENSC/rocket.cc
	$(GCC) $(CXXFLAGS) -c $^ -o $@

$(NAME): $(OBJECTS)
	$(GCC) $(CXXFLAGS) $(DEFINES) $^ $(LIBS) -o $@
