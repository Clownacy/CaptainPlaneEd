NAME	= PlaneEd.exe
GCC	= g++
CFLAGS	= `sdl2-config --cflags` -O3 -s -std=c++11
DEFINES	= 
LIBS	= `sdl2-config --libs`

SOURCES	= PlaneEd.cpp TxtRead.cpp PrjHndl.cpp Tile.cpp Graphics.cpp LevMap.cpp SelRect.cpp
SOURCES	+= compression/EniComp.cpp compression/EniDec.cpp compression/KidDec.cpp compression/KosDec.cpp compression/NemDec.cpp compression/ReadPlain.cpp

$(NAME): $(SOURCES)
	$(GCC) $(CFLAGS) $(DEFINES) $^ $(LIBS) -o $@
