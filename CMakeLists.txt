cmake_minimum_required(VERSION 3.0...3.28.3)

project(CaptainPlaneEd LANGUAGES C CXX)

add_executable(CaptainPlaneEd
	"Common.h"
	"Graphics.cpp"
	"Graphics.h"
	"LevMap.cpp"
	"LevMap.h"
	"PlaneEd.cpp"
	"PrjHndl.cpp"
	"PrjHndl.h"
	"Project.cpp"
	"Project.h"
	"Resource.cpp"
	"Resource.h"
	"Screen.cpp"
	"Screen.h"
	"SelRect.cpp"
	"SelRect.h"
	"Tile.cpp"
	"Tile.h"
	"TxtRead.cpp"
	"TxtRead.h"
	"compression/KidDec.c"
	"compression/KidDec.h"
	"compression/ReadPlain.c"
	"compression/ReadPlain.h"
	"compression/FW_KENSC/basic_decoder.h"
	"compression/FW_KENSC/bigendian_io.h"
	"compression/FW_KENSC/bitstream.h"
	"compression/FW_KENSC/comper.cc"
	"compression/FW_KENSC/comper.h"
	"compression/FW_KENSC/enigma.cc"
	"compression/FW_KENSC/enigma.h"
	"compression/FW_KENSC/ignore_unused_variable_warning.h"
	"compression/FW_KENSC/kosinski.cc"
	"compression/FW_KENSC/kosinski.h"
	"compression/FW_KENSC/lzss.h"
	"compression/FW_KENSC/moduled_adaptor.h"
	"compression/FW_KENSC/nemesis.cc"
	"compression/FW_KENSC/nemesis.h"
	"compression/FW_KENSC/rocket.cc"
	"compression/FW_KENSC/rocket.h"
	"compression/FW_KENSC/saxman.cc"
	"compression/FW_KENSC/saxman.h"
)

if(WIN32)
	target_sources(CaptainPlaneEd PRIVATE
		"WinAPI.cpp"
		"WinAPI.h"
	)
endif()

find_package(SDL2 REQUIRED)
target_link_libraries(CaptainPlaneEd PRIVATE SDL2::SDL2main SDL2::SDL2)
