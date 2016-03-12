NAME	= PlaneEd.exe
GCC	= g++
CFLAGS	= `sdl2-config --cflags` -O3 -s -std=c++11
DEFINES	= 
LIBS	= `sdl2-config --libs`

SOURCES	= PlaneEd.cpp TxtRead.cpp PrjHndl.cpp Tile.cpp Graphics.cpp LevMap.cpp SelRect.cpp
SOURCES	+= compression/EniComp.c compression/EniDec.c compression/KidDec.cpp compression/KosDec.c compression/NemDec.c compression/ReadPlain.c

$(NAME): $(SOURCES)
	$(GCC) $(CFLAGS) $(DEFINES) $^ $(LIBS) -o $@
