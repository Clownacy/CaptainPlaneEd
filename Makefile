NAME	= PlaneEd.exe
GCC	= g++
CFLAGS	= `sdl-config --cflags` -O3
DEFINES	= 
LIBS	= `sdl-config --libs`

$(NAME): PlaneEd.cpp
	$(GCC) $(CFLAGS) $(DEFINES) $^ $(LIBS) -o $@
