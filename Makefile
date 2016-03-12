NAME	= PlaneEd.exe
GCC	= g++
CFLAGS	= `sdl2-config --cflags` -O3 -s -std=c++11
DEFINES	= 
LIBS	= `sdl2-config --libs`

$(NAME): PlaneEd.cpp TxtRead.cpp PrjHndl.cpp Tile.cpp Graphics.cpp
	$(GCC) $(CFLAGS) $(DEFINES) $^ $(LIBS) -o $@
