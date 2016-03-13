NAME	= PlaneEd.exe
GCC	= g++
CFLAGS	= `sdl2-config --cflags` -O3 -s -std=c++11
DEFINES	= 
LIBS	= `sdl2-config --libs`

SOURCES	= PlaneEd.cpp TxtRead.cpp PrjHndl.cpp Tile.cpp Graphics.cpp LevMap.cpp SelRect.cpp
SOURCES	+= compression/EniComp.c compression/KidDec.c compression/ReadPlain.c
SOURCES += FW_KENSC/comper.cc FW_KENSC/enigma.cc FW_KENSC/kosinski.cc FW_KENSC/nemesis.cc FW_KENSC/saxman.cc

$(NAME): $(SOURCES)
	$(GCC) $(CFLAGS) $(DEFINES) $^ $(LIBS) -o $@
