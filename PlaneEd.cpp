#include <cstdio>
#include <cstdlib>
#include <SDL2/SDL.h>

#include "Tile.h"
#include "LevMap.h"
#include "Graphics.h"
#include "SelRect.hpp"
#include "PrjHndl.h"

using namespace std;

const char* const FILE_MAP_TEMP = "tempmap.bin";
const char* const FILE_ART_TEMP = "tempart.bin";
const char* const FILE_PAL_TEMP = "temppal.bin";

int main(int argc, char *argv[])
{
    ProjectData PrjData(argv[1]);
    PrjData.LoadArt(FILE_ART_TEMP);
    PrjData.LoadMap(FILE_MAP_TEMP);
    PrjData.LoadPal(FILE_PAL_TEMP);

    Graphics GfxStuff(PrjData.xSize, PrjData.tileOffset, PrjData.tileAmount);
    LevMap LevelMap(PrjData.xSize, PrjData.ySize, &GfxStuff);

    GfxStuff.ReadPalette(FILE_PAL_TEMP);
    GfxStuff.ReadTiles(FILE_ART_TEMP);
    GfxStuff.CreateTiles();
    
    LevelMap.LoadMap(FILE_MAP_TEMP);
    
    SelRect* SelectionRect = new SelRect(&GfxStuff, &LevelMap);
    SelRect* CopyRect = new SelRect(&GfxStuff, &LevelMap);
    
    bool CtrlPress = false;

    //Process initial display
    LevelMap.DrawMap();
    GfxStuff.DrawSelector();
    
    //SDL_EnableKeyRepeat(400,SDL_DEFAULT_REPEAT_INTERVAL);

    //Main Loop including event Handling
    bool quit = false;
    while(!quit)
    {
        SDL_Event event;

        while((!quit) && SDL_WaitEvent(&event))
        {
            if(event.type == SDL_QUIT) quit = true;
            if(event.type == SDL_MOUSEBUTTONDOWN) 
            {
                SelectionRect->Unselect();
                //Checks if within selector bounds and selects tile
                LevelMap.CheckClickTile(event.button.x, event.button.y);
                if(event.button.button == SDL_BUTTON_LEFT)
                    //Checks if valid map position and sets tile
                    LevelMap.CheckSetTile(event.button.x, event.button.y);
                else if(event.button.button == SDL_BUTTON_RIGHT) {
                    //Checks if valid map position and selects tile
                    LevelMap.CheckSelectTile(event.button.x, event.button.y);
                    SelectionRect->SelInit(event.button.x, event.button.y);
                }
            }
            if(event.type == SDL_MOUSEBUTTONUP) 
            {
                if(event.button.button == SDL_BUTTON_RIGHT) {
                    //Checks if valid map position and selects tile
                    LevelMap.CheckSelectTile(event.button.x, event.button.y);
                    SelectionRect->SelFinalize(event.button.x, event.button.y);
                }
            }
            if(event.type == SDL_MOUSEMOTION)
            {
                if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
                    LevelMap.CheckSetTile(event.motion.x, event.motion.y);
                else {
                    LevelMap.RefreshTileScreen(event.motion.x - event.motion.xrel, event.motion.y - event.motion.yrel, true);
                    LevelMap.DrawSelectedTile(event.motion.x, event.motion.y);
                }
            }
            if(event.type == SDL_KEYDOWN)
            {
                //secondary cases usually for German keyboard layout support
                switch(event.key.keysym.sym)
                {
                    case SDLK_RCTRL:
                    case SDLK_LCTRL:
                        CtrlPress = true; break;
                    case '/':
                    case '-':
                        if(!SelectionRect->isActive()) LevelMap.CurSwapPriority();
                        else {
                            SelectionRect->SwapPriority();
                            LevelMap.DrawMap();
                            SelectionRect->SelDrawRect();
                        }
                        break;
                    case ',':
                        if(!SelectionRect->isActive()) LevelMap.CurFlipX();
                        else {
                            SelectionRect->FlipX();
                            LevelMap.DrawMap();
                            SelectionRect->SelDrawRect();
                        }
                        break;
                    case '.':
                        if(!SelectionRect->isActive()) LevelMap.CurFlipY();
                        else {
                            SelectionRect->FlipY();
                            LevelMap.DrawMap();
                            SelectionRect->SelDrawRect();
                        }
                        break;
                    case SDLK_RIGHT:
                        LevelMap.CurShiftRight();
                        SelectionRect->Unselect(); break;
                    case SDLK_LEFT:
                        LevelMap.CurShiftLeft();
                        SelectionRect->Unselect(); break;
                    case SDLK_DOWN:
                        LevelMap.CurShiftDown();
                        SelectionRect->Unselect(); break;
                    case SDLK_UP:
                        LevelMap.CurShiftUp();
                        SelectionRect->Unselect(); break;
                    case SDLK_DELETE:
                        if(!SelectionRect->isActive()) LevelMap.ClearCurrentTile();
                        else {
                            SelectionRect->clear();
                            SelectionRect->AssignSection();
                            LevelMap.DrawMap();
                        }
                        break;
                    case SDLK_ESCAPE:
                        quit = true; break;
                    case SDLK_RETURN:
                        LevelMap.SetTileSelected();
                        SelectionRect->Unselect(); break;
                    case '=':
                    case '´':
                        LevelMap.SelectTileCur();
                        SelectionRect->Unselect(); break;
                    case SDLK_PAGEDOWN:
                        if(!SelectionRect->isActive()) LevelMap.SelectedTileIncrID();
                        else {
                            SelectionRect->IncrID();
                            SelectionRect->AssignSection();
                            LevelMap.DrawMap();
                        }
                        break;
                    case SDLK_PAGEUP:
                        if(!SelectionRect->isActive()) LevelMap.SelectedTileDecrID();
                        else {
                            SelectionRect->DecrID();
                            SelectionRect->AssignSection();
                            LevelMap.DrawMap();
                        }
                        break;
                    case SDLK_F1:
                        LevelMap.SetPalCurrent(0);
                        GfxStuff.DrawSelector(); break;
                    case SDLK_F2:
                        LevelMap.SetPalCurrent(1);
                        GfxStuff.DrawSelector(); break;
                    case SDLK_F3:
                        LevelMap.SetPalCurrent(2);
                        GfxStuff.DrawSelector(); break;
                    case SDLK_F4:
                        LevelMap.SetPalCurrent(3);
                        GfxStuff.DrawSelector(); break;
                    case SDLK_F5:
                        GfxStuff.ToggleHighPriority();
                        LevelMap.DrawMap();
                        SelectionRect->SelDrawRect(); break;
                    case SDLK_F6:
                        GfxStuff.ToggleLowPriority();
                        LevelMap.DrawMap();
                        SelectionRect->SelDrawRect(); break;
                    case SDLK_F9:
                        LevelMap.SaveMap(FILE_MAP_TEMP);
                        PrjData.SaveMap(FILE_MAP_TEMP); break;
                    case SDLK_F10: //redraw whole screen
                        LevelMap.DrawMap();
                        GfxStuff.DrawSelector();
                        SelectionRect->SelDrawRect(); break;
                    case SDLK_BACKSPACE:
                        LevelMap.CurShiftLeft();
                        LevelMap.ClearCurrentTile(); 
                        SelectionRect->Unselect(); break;
                    case '[':
                    case 'ü':
                        GfxStuff.DecScreenOffset();
                        LevelMap.DrawMap();
                        SelectionRect->SelDrawRect(); break;
                    case '\'':
                    case 'ä':
                        GfxStuff.IncScreenOffset();
                        LevelMap.DrawMap();
                        SelectionRect->SelDrawRect(); break;
                    case ';':
                    case 'ö':
                        GfxStuff.DecScreenXOffset();
                        LevelMap.DrawMap();
                        SelectionRect->SelDrawRect(); break;
                    case '\\':
                    case '#':
                    case '+':
                    case ']':
                        GfxStuff.IncScreenXOffset();
                        LevelMap.DrawMap();
                        SelectionRect->SelDrawRect(); break;
                    case SDLK_END:
                        GfxStuff.IncSelOffset();
                        GfxStuff.DrawSelector(); break;
                    case SDLK_HOME:
                        GfxStuff.DecSelOffset();
                        GfxStuff.DrawSelector(); break;
                    case ' ':
                        LevelMap.ClearCurrentTile();
                        LevelMap.CurShiftRight(); break;
                    default:
                        if(event.key.keysym.sym >= 'a' && event.key.keysym.sym <= 'z') {
                            if(CtrlPress) {
                                switch(event.key.keysym.sym)
                                {
                                    case 'c':
                                        if(SelectionRect->isActive()) {
                                            if(CopyRect != NULL) delete CopyRect;
                                            CopyRect = new SelRect(*SelectionRect);
                                        }
                                        break;
                                    case 'x':
                                        if(SelectionRect->isActive()) {
                                            if(CopyRect != NULL) delete CopyRect;
                                            CopyRect = new SelRect(*SelectionRect);
                                            SelectionRect->clear();
                                            SelectionRect->AssignSection();
                                            LevelMap.DrawMap();
                                        }
                                        break;
                                    case 'v':
                                        CopyRect->PasteSection();
                                        LevelMap.DrawMap();
                                        break;
                                    /*case 'a':
                                        break;*/
                                }
                            } else {
                                LevelMap.SetCurrentTile(event.key.keysym.sym - 'a' + GfxStuff.GetTileOffset() + PrjData.letterOffset);
                                LevelMap.CurShiftRight();
                                SelectionRect->Unselect();
                            }
                        }
                        else if(event.key.keysym.sym >= '0' && event.key.keysym.sym <= '9') {
                            LevelMap.SetCurrentTile(event.key.keysym.sym - '0' + GfxStuff.GetTileOffset() + PrjData.numberOffset);
                            LevelMap.CurShiftRight();
                            SelectionRect->Unselect();
                        }
                }
            }
            if(event.type == SDL_KEYUP)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_RCTRL:
                    case SDLK_LCTRL:
                        CtrlPress = false; break;
                }
            }
            GfxStuff.ProcessDisplay();
        }
    }
    return 0;
}
