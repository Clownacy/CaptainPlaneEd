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

#include <cmath>
#include <cstddef>
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
	this->tile_atlas = nullptr;
	this->atlas_quadrant_dimension = 0;
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

	if (tile_atlas != nullptr)
		SDL_DestroyTexture(tile_atlas);
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

	// Here, we turn the tiles into a tile atlas, divided into four quadrants.
	// Each quadrant is for a different palette line.
	this->atlas_quadrant_dimension = std::ceil(std::sqrt(tileAmount)) * 8;

	SDL_Surface* surface = SDL_CreateRGBSurface(0, this->atlas_quadrant_dimension * 2, this->atlas_quadrant_dimension * 2, 16, 0x0F00, 0x00F0, 0x000F, 0xF000);

	if (surface == NULL)
		MainScreen->ShowInternalError("Cannot make SDL Surface from tiles\n\n", SDL_GetError());

	for (int tile=0; tile < tileAmount; ++tile)
	{
		const std::size_t tile_x = (tile * 8) % this->atlas_quadrant_dimension;
		const std::size_t tile_y = ((tile * 8) / this->atlas_quadrant_dimension) * (surface->pitch / 2 * 8);

		uint16_t *tile_pixels = &((uint16_t*)surface->pixels)[tile_y + tile_x];

		uint16_t *tile_pixels_row[4];
		tile_pixels_row[0] = tile_pixels;
		tile_pixels_row[1] = tile_pixels + this->atlas_quadrant_dimension; // Quadrant to the right
		tile_pixels_row[2] = tile_pixels + (surface->pitch / 2) * this->atlas_quadrant_dimension; // Quadrant below
		tile_pixels_row[3] = tile_pixels_row[2] + this->atlas_quadrant_dimension; // Quadrant below and to the right

		for (std::size_t y = 0; y < 8; ++y)
		{
			for (std::size_t x = 0; x < 8; x += 2)
			{
				const unsigned char byte = fgetc(tilefile);
				const unsigned char nibble1 = (byte >> 4) & 0xF;
				const unsigned char nibble2 = byte & 0xF;

				for (std::size_t i = 0; i < paletteLines; ++i)
				{
					tile_pixels_row[i][x] = palette[i][nibble1];
					tile_pixels_row[i][x + 1] = palette[i][nibble2];
				}
			}

			for (std::size_t i = 0; i < paletteLines; ++i)
				tile_pixels_row[i] += this->atlas_quadrant_dimension * 2;
		}
	}

	this->tile_atlas = SDL_CreateTextureFromSurface(MainScreen->renderer, surface);

	if (this->tile_atlas == NULL)
		MainScreen->ShowInternalError("Cannot make SDL Texture from tiles\n\n", SDL_GetError());

	SDL_FreeSurface(surface);

	fclose(tilefile);
	remove(filename);
}

void Graphics::DrawTileFromAtlas(const int tile_index, SDL_Texture* const screen, const int x, const int y, const int palette_line, bool x_flip, bool y_flip)
{
	SDL_SetRenderTarget(MainScreen->renderer, screen);

	// Construct source rectangle
	SDL_Rect src_rect;
	src_rect.x = (tile_index * 8) % this->atlas_quadrant_dimension;
	src_rect.y = ((tile_index * 8) / this->atlas_quadrant_dimension) * 8;
	src_rect.w = 8;
	src_rect.h = 8;

	// Construct destination rectangle
	SDL_Rect dst_rect;
	dst_rect.x = x;
	dst_rect.y = y;
	dst_rect.w = 8;
	dst_rect.h = 8;

	// The atlas is divided into quadrants, each representing a different palette line
	if (palette_line & 1)
		src_rect.x += this->atlas_quadrant_dimension;
	if (palette_line & 2)
		src_rect.y += this->atlas_quadrant_dimension;

	// Handle flipping
	int flip = SDL_FLIP_NONE;
	if (x_flip)
		flip |= SDL_FLIP_HORIZONTAL;
	if (y_flip)
		flip |= SDL_FLIP_VERTICAL;

	// Finally render
	SDL_RenderCopyEx(MainScreen->renderer, this->tile_atlas, &src_rect, &dst_rect, 0.0, nullptr, (SDL_RendererFlip)flip);
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
		DrawTileFromAtlas(i, MainScreen->texture, selXMin + 8*(i%selectorWidth), 8*(i/selectorWidth - selTileYOffset), currentPal, false, false);
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
				DrawTileFromAtlas((tile->tileID) - tileOffset, MainScreen->texture, 8*x, 8*y, tile->paletteLine, tile->xFlip, tile->yFlip);
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
