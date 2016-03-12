NAME	= PlaneEd.exe
GCC	= g++
CFLAGS	= `sdl2-config --cflags` -O3 -s
DEFINES	= 
LIBS	= `sdl2-config --libs`

$(NAME): PlaneEd.cpp TxtRead.cpp
	$(GCC) $(CFLAGS) $(DEFINES) $^ $(LIBS) -o $@
