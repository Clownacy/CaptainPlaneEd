#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "Graphics.h"
#include <algorithm>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480   //minimum size to allow for 64x64 maps

Graphics::Graphics(Uint16 xSize, Uint16 tileOffset, Uint16 tileAmount) {
    this->selXMin = std::min(8*64, 8*xSize) + 1;
    this->xDisplaySize = std::min(64, 0+xSize);
    this->tileOffset = tileOffset;
    this->tileAmount = tileAmount;
    this->currentPal = 0;
    this->highPriorityDisplay = true;
    this->lowPriorityDisplay = true;
    this->screenTileYOffset = 0;
    this->screenTileXOffset = 0;
    this->selTileYOffset = 0;
    
    /* calculate selector width */
    this->selectorWidth = 8;
    while(8*tileAmount / selectorWidth > SCREEN_HEIGHT) {
        if(8 * (xSize+selectorWidth) < SCREEN_WIDTH) selectorWidth += 8;
        else break;
    }

    if(SDL_Init(SDL_INIT_VIDEO)<0) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
    
    screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,32,SDL_HWSURFACE|SDL_DOUBLEBUF);
    if(screen==NULL) {
        fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
        exit(1);
    }
    
    SDL_WM_SetCaption("PlaneEd", NULL); 
}

void Graphics::ReadPalette(char* filename) {
    FILE* palfile = fopen(filename,"rb");
    if(palfile==NULL) {
        fprintf(stderr, "Cannot open palette file.\n");
        exit(1);
    }
    fseek(palfile, 0, SEEK_END);
    paletteLines = ftell(palfile)/0x20;
    if(paletteLines > 4) paletteLines = 4;
    rewind(palfile);
    if(paletteLines == 0) {
        fprintf(stderr, "Palette file too small.\n");
        exit(1);        
    }
    palette = new Uint16[paletteLines][16];
    for(int i=0; i<paletteLines; i++)
        fread(palette[i], sizeof(unsigned char), 32, palfile);
    fclose(palfile);
    remove(filename);
}

void Graphics::ReadTiles(char* filename) {
    FILE* tilefile = fopen(filename,"rb");
    if(tilefile==NULL) {
        fprintf(stderr, "Cannot open tile graphics file.\n");
        exit(1);
    }
    unsigned char tilebuffer[32]; //space for one tile
    tileData = new Uint16***[tileAmount];
    for(int t=0; t<tileAmount; t++) {
        fread(tilebuffer, sizeof(unsigned char), 32, tilefile);
        tileData[t] = new Uint16**[paletteLines];
        for(int p=0; p<paletteLines; p++) {
            tileData[t][p] = new Uint16*[4];
            for(int f=0; f<4; f++) tileData[t][p][f] = new Uint16[64];
            for(int i=0; i<32; i++) {
                tileData[t][p][0][2*i]   = palette[p][(tilebuffer[i] & 0xF0)>>4];
                tileData[t][p][0][2*i+1] = palette[p][(tilebuffer[i] & 0x0F)];
                tileData[t][p][1][8*(i/4)+7-2*(i%4)]   = palette[p][(tilebuffer[i] & 0xF0)>>4]; //X-flip
                tileData[t][p][1][8*(i/4)+7-2*(i%4)-1] = palette[p][(tilebuffer[i] & 0x0F)];
                tileData[t][p][2][56-8*(i/4)+2*(i%4)]   = palette[p][(tilebuffer[i] & 0xF0)>>4]; //Y-flip
                tileData[t][p][2][56-8*(i/4)+2*(i%4)+1] = palette[p][(tilebuffer[i] & 0x0F)];
                tileData[t][p][3][63-2*i]   = palette[p][(tilebuffer[i] & 0xF0)>>4]; //XY-flip
                tileData[t][p][3][63-2*i-1] = palette[p][(tilebuffer[i] & 0x0F)];
            }
        }
    }
    fclose(tilefile);
    remove(filename);
}

void Graphics::CreateTiles(){
    tiles = new SDL_Surface***[tileAmount];
    for(int t=0; t<tileAmount; t++)
    {
        tiles[t] = new SDL_Surface**[paletteLines];
        for(int p=0; p<paletteLines; p++)
        {
            tiles[t][p] = new SDL_Surface*[4];
            for(int f=0; f<4; f++)
                tiles[t][p][f] = InitSurface(tileData[t][p][f], 8, 8, 16);
        }
    }
}

SDL_Surface* Graphics::InitSurface(Uint16 *pixelsT, int width, int height, int bbp) {
    void* pixels = pixelsT;
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom (pixels, width,
                         height, bbp, width*((bbp+7)/8), 0x0F00, 0xF000, 0x000F, 0);
    return(surface);
}

void Graphics::DrawSurface(SDL_Surface *img, SDL_Surface *screen, int x, int y) {
    SDL_Rect RectTemp;
    RectTemp.x = x;
    RectTemp.y = y;
    SDL_BlitSurface(img, NULL, screen, &RectTemp);
}

void Graphics::ClearMap() {
    Uint32 color = SDL_MapRGB(screen->format, 0, 0, 0);
    SDL_Rect RectTemp;
    RectTemp.x = 0;
    RectTemp.y = 0;
    RectTemp.w = selXMin-1;
    RectTemp.h = SCREEN_HEIGHT;
    SDL_FillRect(this->screen, &RectTemp, color);    
}

void Graphics::ClearSelector() {
    Uint32 color = SDL_MapRGB(screen->format, 0, 0, 0);
    SDL_Rect RectTemp;
    RectTemp.x = selXMin;
    RectTemp.y = 0;
    RectTemp.w = 8*selectorWidth;
    RectTemp.h = SCREEN_HEIGHT;
    SDL_FillRect(this->screen, &RectTemp, color);
}

void Graphics::DrawSelector() {
    ClearSelector();
    for(int i=0; i<tileAmount; i++)
        DrawSurface(tiles[i][currentPal][0], this->screen, selXMin + 8*(i%selectorWidth), 8*(i/selectorWidth - selTileYOffset));
    SDL_Flip(screen);
}

/* map coords */
void Graphics::DrawTileSingle(int x, int y, Tile tile) {
    y -= screenTileYOffset;
    x -= screenTileXOffset;
    if (x >= xDisplaySize) return;
    if((tile.priority && highPriorityDisplay) || (!tile.priority && lowPriorityDisplay)) {
        if((tile.tileID) - tileOffset >= this->tileAmount) {
            DrawTileNone(x, y);
            DrawTileInvalid(x, y);
        } else if((tile.tileID || !this->tileOffset) && tile.paletteLine < paletteLines)
            DrawSurface(tiles[(tile.tileID) - tileOffset][tile.paletteLine][(tile.xFlip | tile.yFlip)>>3], screen, 8*x, 8*y);
        else DrawTileBlank(x, y, tile);
    } else DrawTileNone(x, y);
}

bool Graphics::CheckSelValidPos(int x, int y) {
    if(x>=selXMin && x<selXMin+8*selectorWidth && y>=0 && (x-selXMin)/8+selectorWidth*(y/8 + selTileYOffset)<GetTileAmount()) return true;
    else return false;
}

void Graphics::DrawTileNone(int x, int y) {
    Uint32 color = SDL_MapRGB(screen->format, 0, 0, 0);
    DrawTileFullColor(x, y, color);
}

void Graphics::DrawTileBlank(int x, int y, Tile tile) {
    Uint32 color = SDL_MapRGB(
        screen->format,
        (palette[tile.paletteLine][0] & 0x0F00)>>4,
        (palette[tile.paletteLine][0] & 0x00F0),
        (palette[tile.paletteLine][0] & 0x000F)<<4
    );
    DrawTileFullColor(x, y, color);
}

void Graphics::DrawTileFullColor(int x, int y, Uint32 color) {
    SDL_Rect RectTemp;
    RectTemp.x = 8*x;
    RectTemp.y = 8*y;
    RectTemp.w = 8;
    RectTemp.h = 8;
    SDL_FillRect(this->screen, &RectTemp, color);
}

void Graphics::DrawTileInvalid(int x, int y) {
    //PosTileToScreen(x, y);
    for(int i=0; i<8; i++) {
        DrawPixel(8*x+i, 8*y+i);
        DrawPixel(8*x+i, 8*y+7-i);
    }
}

void Graphics::DrawPixel(int x, int y) {
    if(x<0 || x>=SCREEN_WIDTH || y<0 || y>=SCREEN_HEIGHT) return;
    Uint32 color = SDL_MapRGB(screen->format, 0xE0, 0xB0, 0xD0);

    Uint32 *pixel;
    pixel = (Uint32*) screen->pixels + y*screen->pitch/4 + x;
    *pixel = color;
}

/* map coords */
void Graphics::DrawRect(int x, int y) {
    PosTileToScreen(x, y);
    for(int i=0; i<8; i++) {
        DrawPixel(x+i, y);
        DrawPixel(x+i, y+7);
        DrawPixel(x, y+i);
        DrawPixel(x+7, y+i);
    }
}

/* map coords */
void Graphics::DrawFreeRect(int x, int y, int xSize, int ySize) {
    PosTileToScreen(x, y);
    for(int i=0; i<8*ySize; i++) {
        DrawPixel(x, y+i);
        DrawPixel(x + 8*xSize - 1, y+i);
    }
    for(int i=0; i<8*xSize; i++) {
        DrawPixel(x+i, y);
        DrawPixel(x+i, y + 8*ySize - 1);
    }
}

void Graphics::PosScreenToTile(int& x, int& y) {
    x /= 8;
    y /= 8;
    y += screenTileYOffset;
    x += screenTileXOffset;
}

void Graphics::PosScreenToTileRound(int& x, int& y) {
    x = (x+4)/8;
    y = (y+4)/8;
    y += screenTileYOffset;
    x += screenTileXOffset;
}

void Graphics::PosTileToScreen(int& x, int& y) {
    y -= screenTileYOffset;
    x -= screenTileXOffset;
    x *= 8;
    y *= 8;
}

#endif
