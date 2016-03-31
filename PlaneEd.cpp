#include <cstdio>
#include <cstdlib>
#include <SDL2/SDL.h>

#include "Common.h"
#include "Tile.h"
#include "LevMap.h"
#include "Graphics.h"
#include "Screen.h"
#include "SelRect.h"
#include "Project.h"
#include "PrjHndl.h"
#include "Resource.h"
#include "Windows.h"

ProjectData* Current_ProjectData;
LevMap* Current_LevelMap;

int main(int argc, char* argv[])
{
    Screen MainScreen;
    Project Project1(argv[1], &MainScreen);
    
    bool CtrlPress = false;

    //Process initial display
    Project1.LevelMap->DrawMap();
    Project1.GfxStuff->DrawSelector();
    
    //SDL_EnableKeyRepeat(400,SDL_DEFAULT_REPEAT_INTERVAL);

    //Main Loop including event Handling
    bool quit = false;
    while (!quit)
    {
        SDL_Event event;

        while ((!quit) && SDL_WaitEvent(&event))
        {
            if (event.type == SDL_QUIT) quit = true;
            if (event.type == SDL_MOUSEBUTTONDOWN) 
            {
                Project1.SelectionRect->Unselect();
                //Checks if within selector bounds and selects tile
                Project1.LevelMap->CheckClickTile(event.button.x, event.button.y);
                if (event.button.button == SDL_BUTTON_LEFT)
                    //Checks if valid map position and sets tile
                    Project1.LevelMap->CheckSetTile(event.button.x, event.button.y);
                else if (event.button.button == SDL_BUTTON_RIGHT) {
                    //Checks if valid map position and selects tile
                    Project1.LevelMap->CheckSelectTile(event.button.x, event.button.y);
                    Project1.SelectionRect->SelInit(event.button.x, event.button.y);
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP) 
            {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    //Checks if valid map position and selects tile
                    Project1.LevelMap->CheckSelectTile(event.button.x, event.button.y);
                    Project1.SelectionRect->SelFinalize(event.button.x, event.button.y);
                }
            }
            if (event.type == SDL_MOUSEMOTION)
            {
                if (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
                    Project1.LevelMap->CheckSetTile(event.motion.x, event.motion.y);
                else {
                    Project1.LevelMap->RefreshTileScreen(event.motion.x - event.motion.xrel, event.motion.y - event.motion.yrel, true);
                    Project1.LevelMap->DrawSelectedTile(event.motion.x, event.motion.y);
                }
            }
            if (event.type == SDL_KEYDOWN)
            {
                //secondary cases usually for German keyboard layout support
                switch(event.key.keysym.sym)
                {
                    case SDLK_RCTRL:
                    case SDLK_LCTRL:
                        CtrlPress = true; break;
                    case '/':
                    case '-':
                        if (!Project1.SelectionRect->isActive()) Project1.LevelMap->CurSwapPriority();
                        else {
                            Project1.SelectionRect->SwapPriority();
                            Project1.LevelMap->DrawMap();
                            Project1.SelectionRect->SelDrawRect();
                        }
                        break;
                    case ',':
                        if (!Project1.SelectionRect->isActive()) Project1.LevelMap->CurFlipX();
                        else {
                            Project1.SelectionRect->FlipX();
                            Project1.LevelMap->DrawMap();
                            Project1.SelectionRect->SelDrawRect();
                        }
                        break;
                    case '.':
                        if (!Project1.SelectionRect->isActive()) Project1.LevelMap->CurFlipY();
                        else {
                            Project1.SelectionRect->FlipY();
                            Project1.LevelMap->DrawMap();
                            Project1.SelectionRect->SelDrawRect();
                        }
                        break;
                    case SDLK_RIGHT:
                        Project1.LevelMap->CurShiftRight();
                        Project1.SelectionRect->Unselect(); break;
                    case SDLK_LEFT:
                        Project1.LevelMap->CurShiftLeft();
                        Project1.SelectionRect->Unselect(); break;
                    case SDLK_DOWN:
                        Project1.LevelMap->CurShiftDown();
                        Project1.SelectionRect->Unselect(); break;
                    case SDLK_UP:
                        Project1.LevelMap->CurShiftUp();
                        Project1.SelectionRect->Unselect(); break;
                    case SDLK_DELETE:
                        if (!Project1.SelectionRect->isActive()) Project1.LevelMap->ClearCurrentTile();
                        else {
                            Project1.SelectionRect->clear();
                            Project1.SelectionRect->AssignSection();
                            Project1.LevelMap->DrawMap();
                        }
                        break;
                    case SDLK_ESCAPE:
                        quit = true; break;
                    case SDLK_RETURN:
                        Project1.LevelMap->SetTileSelected();
                        Project1.SelectionRect->Unselect(); break;
                    case '=':
                    case '´':
                        Project1.LevelMap->SelectTileCur();
                        Project1.SelectionRect->Unselect(); break;
                    case SDLK_PAGEDOWN:
                        if (!Project1.SelectionRect->isActive()) Project1.LevelMap->SelectedTileIncrID();
                        else {
                            Project1.SelectionRect->IncrID();
                            Project1.SelectionRect->AssignSection();
                            Project1.LevelMap->DrawMap();
                        }
                        break;
                    case SDLK_PAGEUP:
                        if (!Project1.SelectionRect->isActive()) Project1.LevelMap->SelectedTileDecrID();
                        else {
                            Project1.SelectionRect->DecrID();
                            Project1.SelectionRect->AssignSection();
                            Project1.LevelMap->DrawMap();
                        }
                        break;
                    case SDLK_F1:
                        Project1.LevelMap->SetPalCurrent(0);
                        Project1.GfxStuff->DrawSelector(); break;
                    case SDLK_F2:
                        Project1.LevelMap->SetPalCurrent(1);
                        Project1.GfxStuff->DrawSelector(); break;
                    case SDLK_F3:
                        Project1.LevelMap->SetPalCurrent(2);
                        Project1.GfxStuff->DrawSelector(); break;
                    case SDLK_F4:
                        Project1.LevelMap->SetPalCurrent(3);
                        Project1.GfxStuff->DrawSelector(); break;
                    case SDLK_F5:
                        Project1.GfxStuff->ToggleHighPriority();
                        Project1.LevelMap->DrawMap();
                        Project1.SelectionRect->SelDrawRect(); break;
                    case SDLK_F6:
                        Project1.GfxStuff->ToggleLowPriority();
                        Project1.LevelMap->DrawMap();
                        Project1.SelectionRect->SelDrawRect(); break;
                    case SDLK_F9:
                        Project1.LevelMap->SaveMap(FILE_MAP_TEMP);
			Project1.PrjData->map.Save(FILE_MAP_TEMP, Project1.PrjData->map.saveName);
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Information", "Save complete.", NULL);
			break;
                    case SDLK_F10: //redraw whole screen
                        Project1.LevelMap->DrawMap();
                        Project1.GfxStuff->DrawSelector();
                        Project1.SelectionRect->SelDrawRect(); break;
                    case SDLK_BACKSPACE:
                        Project1.LevelMap->CurShiftLeft();
                        Project1.LevelMap->ClearCurrentTile(); 
                        Project1.SelectionRect->Unselect(); break;
                    case '[':
                    case 'ü':
                        Project1.GfxStuff->DecScreenOffset();
                        Project1.LevelMap->DrawMap();
                        Project1.SelectionRect->SelDrawRect(); break;
                    case '\'':
                    case 'ä':
                        Project1.GfxStuff->IncScreenOffset();
                        Project1.LevelMap->DrawMap();
                        Project1.SelectionRect->SelDrawRect(); break;
                    case ';':
                    case 'ö':
                        Project1.GfxStuff->DecScreenXOffset();
                        Project1.LevelMap->DrawMap();
                        Project1.SelectionRect->SelDrawRect(); break;
                    case '\\':
                    case '#':
                    case '+':
                    case ']':
                        Project1.GfxStuff->IncScreenXOffset();
                        Project1.LevelMap->DrawMap();
                        Project1.SelectionRect->SelDrawRect(); break;
                    case SDLK_END:
                        Project1.GfxStuff->IncSelOffset();
                        Project1.GfxStuff->DrawSelector(); break;
                    case SDLK_HOME:
                        Project1.GfxStuff->DecSelOffset();
                        Project1.GfxStuff->DrawSelector(); break;
                    case ' ':
                        Project1.LevelMap->ClearCurrentTile();
                        Project1.LevelMap->CurShiftRight(); break;
                    default:
                        if (event.key.keysym.sym >= 'a' && event.key.keysym.sym <= 'z') {
                            if (CtrlPress) {
                                switch(event.key.keysym.sym)
                                {
                                    case 'c':
                                        if (Project1.SelectionRect->isActive()) {
                                            if (Project1.CopyRect != NULL) delete Project1.CopyRect;
                                            Project1.CopyRect = new SelRect(Project1.SelectionRect);
                                        }
                                        break;
                                    case 'x':
                                        if (Project1.SelectionRect->isActive()) {
                                            if (Project1.CopyRect != NULL) delete Project1.CopyRect;
                                            Project1.CopyRect = new SelRect(Project1.SelectionRect);
                                            Project1.SelectionRect->clear();
                                            Project1.SelectionRect->AssignSection();
                                            Project1.LevelMap->DrawMap();
                                        }
                                        break;
                                    case 'v':
                                        Project1.CopyRect->PasteSection();
                                        Project1.LevelMap->DrawMap();
                                        break;
                                    /*case 'a':
                                        break;*/
                                }
                            } else {
                                Project1.LevelMap->SetCurrentTile(event.key.keysym.sym - 'a' + Project1.GfxStuff->GetTileOffset() + Project1.PrjData->letterOffset);
                                Project1.LevelMap->CurShiftRight();
                                Project1.SelectionRect->Unselect();
                            }
                        }
                        else if (event.key.keysym.sym >= '0' && event.key.keysym.sym <= '9') {
                            Project1.LevelMap->SetCurrentTile(event.key.keysym.sym - '0' + Project1.GfxStuff->GetTileOffset() + Project1.PrjData->numberOffset);
                            Project1.LevelMap->CurShiftRight();
                            Project1.SelectionRect->Unselect();
                        }
                }
            }
            if (event.type == SDL_KEYUP)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_RCTRL:
                    case SDLK_LCTRL:
                        CtrlPress = false; break;
                }
            }
            if (event.type == SDL_SYSWMEVENT)
	    {
		    WinAPI::HandleWindowsEvent(&event);
	    }
            MainScreen.ProcessDisplay();
        }
    }
    return 0;
}
