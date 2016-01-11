NAME	= PlaneEd.exe
GCC	= g++
CFLAGS	= `sdl2-config --cflags` -O3
DEFINES	= 
LIBS	= `sdl2-config --libs`

$(NAME): PlaneEd.cpp
	$(GCC) $(CFLAGS) $(DEFINES) $^ $(LIBS) -o $@
