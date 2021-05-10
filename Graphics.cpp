/*
	Copyright (C) 2009-2011 qiuu
	Copyright (C) 2016 Clownacy

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/

#include <algorithm>
#include <SDL2/SDL.h>

#include "Common.h"
#include "Graphics.h"
#include "Screen.h"
#ifdef _WIN32
#include "WinAPI.h"
#endif

#define PALETTE_ENTRIES_PER_LINE 16

Graphics::Graphics(const uint16_t xSize, const uint16_t tileOffset, const uint16_t tileAmount)
{
	this->selXMin = std::min(8*64, 8*xSize) + 1;
	this->xDisplaySize = std::min(64, 0+xSize);
	this->tileOffset = tileOffset;
	this->tileAmount = tileAmount;
	SetCurrentPal(0);
	this->highPriorityDisplay = true;
	this->lowPriorityDisplay = true;
	this->screenTileYOffset = 0;
	this->screenTileXOffset = 0;
	this->selTileYOffset = 0;
	this->tiles = nullptr;
	this->tileData = nullptr;
	this->paletteLines = 0;
	
	/* calculate selector width */
	this->selectorWidth = 8;
	while (8*tileAmount / selectorWidth > SCREEN_HEIGHT)
	{
		if (8 * (xSize+selectorWidth) < SCREEN_WIDTH) selectorWidth += 8;
		else break;
	}
}

Graphics::~Graphics(void)
{
#ifdef _WIN32
	WinAPI::SetMenuBarOptionChecked(false, MENUBAR_PALETTELINE1);
	WinAPI::SetMenuBarOptionChecked(false, MENUBAR_PALETTELINE2);
	WinAPI::SetMenuBarOptionChecked(false, MENUBAR_PALETTELINE3);
	WinAPI::SetMenuBarOptionChecked(false, MENUBAR_PALETTELINE4);
#endif
}

void Graphics::ReadPalette(const char* const filename)
{
	FILE* palfile = fopen(filename,"rb");
	if (palfile==NULL)
		MainScreen->ShowInternalError("Decompressed palette file not found");

	fseek(palfile, 0, SEEK_END);
	paletteLines = ftell(palfile)/(PALETTE_ENTRIES_PER_LINE*sizeof(uint16_t));
	rewind(palfile);

	if (paletteLines > 4)
		paletteLines = 4;
	else if (paletteLines == 0)
		MainScreen->ShowError("Palette file too small: it must contain at least one palette line");

	palette = new uint16_t[paletteLines][PALETTE_ENTRIES_PER_LINE];
	for (int line=0; line < paletteLines; ++line)
	{
		for (int entry=0; entry < PALETTE_ENTRIES_PER_LINE; ++entry)
		{
			// Convert BGR to RGB
			const uint16_t palette_entry = (fgetc(palfile)<<8)|fgetc(palfile);
			const uint16_t blue = (palette_entry&0x0F00) >> 8;
			const uint16_t green = palette_entry&0x00F0;
			const uint16_t red = (palette_entry&0x000F) << 8;
			const uint16_t alpha = 0xF000;
			palette[line][entry] = alpha|red|green|blue;
		}
	}

	fclose(palfile);
	remove(filename);
}

void Graphics::ReadTiles(const char* const filename)
{
	FILE* tilefile = fopen(filename,"rb");
	if (tilefile==NULL)
		MainScreen->ShowInternalError("Decompressed art file not found");

	uint8_t tilebuffer[(8*8)/2]; //space for one tile
	tileData = new uint16_t***[tileAmount];
	for (int tile=0; tile < tileAmount; ++tile)
	{
		fread(tilebuffer, sizeof(uint8_t), (8*8)/2, tilefile);
		tileData[tile] = new uint16_t**[paletteLines];
		for (int pal_line=0; pal_line < paletteLines; ++pal_line)
	{
			tileData[tile][pal_line] = new uint16_t*[4];
			for (int flip=0; flip < 4; ++flip)
			{
					tileData[tile][pal_line][flip] = new uint16_t[8*8];
			}
			for (int i=0; i < (8*8)/2; ++i)
			{
				// Normal tile
				tileData[tile][pal_line][0][2*i]   = palette[pal_line][(tilebuffer[i] & 0xF0)>>4];
				tileData[tile][pal_line][0][2*i+1] = palette[pal_line][(tilebuffer[i] & 0x0F)];
				// X-flipped tile
				tileData[tile][pal_line][1][8*(i/4)+7-2*(i%4)]   = palette[pal_line][(tilebuffer[i] & 0xF0)>>4];
				tileData[tile][pal_line][1][8*(i/4)+7-2*(i%4)-1] = palette[pal_line][(tilebuffer[i] & 0x0F)];
				// Y-flipped tile
				tileData[tile][pal_line][2][56-8*(i/4)+2*(i%4)]   = palette[pal_line][(tilebuffer[i] & 0xF0)>>4];
				tileData[tile][pal_line][2][56-8*(i/4)+2*(i%4)+1] = palette[pal_line][(tilebuffer[i] & 0x0F)];
				// X-flipped + Y-flipped tile
				tileData[tile][pal_line][3][63-2*i]   = palette[pal_line][(tilebuffer[i] & 0xF0)>>4];
				tileData[tile][pal_line][3][63-2*i-1] = palette[pal_line][(tilebuffer[i] & 0x0F)];
			}
		}
	}
	fclose(tilefile);
	remove(filename);
}

void Graphics::CreateTiles(void)
{
	tiles = new SDL_Texture***[tileAmount];
	for (int t=0; t < tileAmount; ++t)
	{
		tiles[t] = new SDL_Texture**[paletteLines];
		for (int p=0; p < paletteLines; ++p)
		{
			tiles[t][p] = new SDL_Texture*[4];
			for (int f=0; f < 4; ++f)
			{
				tiles[t][p][f] = InitSurface(tileData[t][p][f], 8, 8, 16);
			}
		}
	}
}

SDL_Texture* Graphics::InitSurface(uint16_t* const pixelsT, const int width, const int height, const int bbp)
{
	void* const pixels = pixelsT;
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom (pixels, width, height, bbp, width*((bbp+7)/8), 0x0F00, 0x00F0, 0x000F, 0xF000);

	if (surface == NULL)
		MainScreen->ShowInternalError("Cannot make SDL Surface from tiles\n\n", SDL_GetError());

	SDL_Texture* texture = SDL_CreateTextureFromSurface(MainScreen->renderer, surface);

	if (texture == NULL)
		MainScreen->ShowInternalError("Cannot make SDL Texture from tiles\n\n", SDL_GetError());

	SDL_FreeSurface(surface);

	return texture;
}

void Graphics::DrawSurface(SDL_Texture* const img, SDL_Texture* const screen, const int x, const int y)
{
	SDL_SetRenderTarget(MainScreen->renderer, screen);

	SDL_Rect RectTemp;
	RectTemp.x = x;
	RectTemp.y = y;
	SDL_QueryTexture(img, NULL, NULL, &RectTemp.w, &RectTemp.h);
	SDL_RenderCopy(MainScreen->renderer, img, NULL, &RectTemp);
}

void Graphics::ClearMap(void)
{
	SDL_SetRenderTarget(MainScreen->renderer, MainScreen->texture);

	SDL_Rect RectTemp;
	RectTemp.x = 0;
	RectTemp.y = 0;
	RectTemp.w = selXMin-1;
	RectTemp.h = SCREEN_HEIGHT;
	SDL_SetRenderDrawColor(MainScreen->renderer, MainScreen->background_colour.r, MainScreen->background_colour.g, MainScreen->background_colour.b, 0xFF);
	SDL_RenderFillRect(MainScreen->renderer, &RectTemp);
}

void Graphics::ClearSelector(void)
{
	SDL_SetRenderTarget(MainScreen->renderer, MainScreen->texture);

	SDL_Rect RectTemp;
	RectTemp.x = selXMin;
	RectTemp.y = 0;
	RectTemp.w = 8*selectorWidth;
	RectTemp.h = SCREEN_HEIGHT;
	SDL_SetRenderDrawColor(MainScreen->renderer, MainScreen->background_colour.r, MainScreen->background_colour.g, MainScreen->background_colour.b, 0xFF);
	SDL_RenderFillRect(MainScreen->renderer, &RectTemp);
}

void Graphics::DrawSelector(void)
{
	ClearSelector();
	for (int i=0; i < tileAmount; ++i)
		DrawSurface(tiles[i][currentPal][0], MainScreen->texture, selXMin + 8*(i%selectorWidth), 8*(i/selectorWidth - selTileYOffset));
}

/* map coords */
void Graphics::DrawTileSingle(int x, int y, const Tile* const tile)
{
	y -= screenTileYOffset;
	x -= screenTileXOffset;
	if (x < xDisplaySize)
	{
		if ((tile->priority && highPriorityDisplay) || (!tile->priority && lowPriorityDisplay))
		{
			if ((tile->tileID) - tileOffset >= this->tileAmount)
			{
				DrawTileNone(x, y);
				DrawTileInvalid(x, y);
			}
			else if ((tile->tileID || !this->tileOffset) && tile->paletteLine < paletteLines)
			{
				DrawSurface(tiles[(tile->tileID) - tileOffset][tile->paletteLine][tile->xFlip | (tile->yFlip<<1)], MainScreen->texture, 8*x, 8*y);
			}
			else
			{
				DrawTileBlank(x, y, tile);
			}
		}
		else
		{
			DrawTileNone(x, y);
		}
	}
}

bool Graphics::CheckSelValidPos(const int x, const int y)
{
	return (x>=selXMin && x<selXMin+8*selectorWidth && y>=0 && (x-selXMin)/8+selectorWidth*(y/8 + selTileYOffset)<GetTileAmount());
}

void Graphics::DrawTileNone(const int x, const int y)
{
	DrawTileFullColor(x, y, MainScreen->background_colour.r, MainScreen->background_colour.g, MainScreen->background_colour.b);
}

void Graphics::DrawTileBlank(const int x, const int y, const Tile* const tile)
{
	DrawTileFullColor(x, y, (palette[tile->paletteLine][0] & 0x0F00)>>4, (palette[tile->paletteLine][0] & 0x00F0), (palette[tile->paletteLine][0] & 0x000F)<<4);
}

void Graphics::DrawTileFullColor(const int x, const int y, const unsigned char red, const unsigned char green, const unsigned char blue)
{
	SDL_SetRenderTarget(MainScreen->renderer, MainScreen->texture);

	SDL_Rect RectTemp;
	RectTemp.x = 8*x;
	RectTemp.y = 8*y;
	RectTemp.w = 8;
	RectTemp.h = 8;
	SDL_SetRenderDrawColor(MainScreen->renderer, red, green, blue, 0xFF);
	SDL_RenderFillRect(MainScreen->renderer, &RectTemp);
}

void Graphics::DrawTileInvalid(const int x, const int y)
{
	SDL_SetRenderTarget(MainScreen->renderer, MainScreen->texture);

	// TODO - restore the cross effect
	//PosTileToScreen(&x, &y);
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = 8;
	rect.h = 8;
	SDL_SetRenderDrawColor(MainScreen->renderer, 0xE0, 0xB0, 0xD0, 0xFF);
	SDL_RenderDrawRect(MainScreen->renderer, &rect);
}

void Graphics::SetCurrentPal(const uint8_t currentPal)
{
	this->currentPal = currentPal;

#ifdef _WIN32
	WinAPI::SetMenuBarOptionChecked(((currentPal == 0) ? true : false), MENUBAR_PALETTELINE1);
	WinAPI::SetMenuBarOptionChecked(((currentPal == 1) ? true : false), MENUBAR_PALETTELINE2);
	WinAPI::SetMenuBarOptionChecked(((currentPal == 2) ? true : false), MENUBAR_PALETTELINE3);
	WinAPI::SetMenuBarOptionChecked(((currentPal == 3) ? true : false), MENUBAR_PALETTELINE4);
#endif
}

/* map coords */
void Graphics::DrawRect(int x, int y)
{
	SDL_SetRenderTarget(MainScreen->renderer, MainScreen->texture);

	PosTileToScreen(&x, &y);
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = 8;
	rect.h = 8;
	SDL_SetRenderDrawColor(MainScreen->renderer, 0xE0, 0xB0, 0xD0, 0xFF);
	SDL_RenderDrawRect(MainScreen->renderer, &rect);
}

/* map coords */
void Graphics::DrawFreeRect(int x, int y, const int xSize, const int ySize)
{
	SDL_SetRenderTarget(MainScreen->renderer, MainScreen->texture);

	PosTileToScreen(&x, &y);
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = 8*xSize;
	rect.h = 8*ySize;
	SDL_SetRenderDrawColor(MainScreen->renderer, 0xE0, 0xB0, 0xD0, 0xFF);
	SDL_RenderDrawRect(MainScreen->renderer, &rect);
}

void Graphics::PosScreenToTile(int* const x, int* const y)
{
	*x /= 8;
	*y /= 8;
	*y += screenTileYOffset;
	*x += screenTileXOffset;
}

void Graphics::PosScreenToTileRound(int* const x, int* const y)
{
	*x = ((*x)+4)/8;
	*y = ((*y)+4)/8;
	*y += screenTileYOffset;
	*x += screenTileXOffset;
}

void Graphics::PosTileToScreen(int* const x, int* const y)
{
	*y -= screenTileYOffset;
	*x -= screenTileXOffset;
	*x *= 8;
	*y *= 8;
}
