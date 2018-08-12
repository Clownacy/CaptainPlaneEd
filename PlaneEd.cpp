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
#ifdef _WIN32
#include "WinAPI.h"
#endif

Screen* MainScreen;
Project* CurProject = NULL;

int main(int argc, char* argv[])
{
    // Create our window
    MainScreen = new Screen;

    MainScreen->Clear();

    // For backwards-compatibility, we want project files that are drag-and-dropped
    // onto the executable to be automatically loaded when it starts.
    // First, we have to make sure the file's actually there.
    FILE* prjfile = (argc > 1) ? fopen(argv[1], "r") : NULL;
#ifdef _WIN32
    // Windows build allows a blank window to open, where the user can open a
    // project file using the menu bar
    if (prjfile != NULL)
    {
	fclose(prjfile);
        CurProject = new Project(argv[1]);

        // Process initial display
        CurProject->Redraw();
    }
#else
    if (prjfile == NULL)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "You must supply the project file as a parameter", NULL);
        exit(1);
    }
    fclose(prjfile);
    CurProject = new Project(argv[1]);

    // Process initial display
    CurProject->Redraw();
#endif

    bool CtrlPress = false;
    
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
		if (CurProject != NULL)
		{
                    CurProject->SelectionRect->Unselect();
                    //Checks if within selector bounds and selects tile
                    CurProject->LevelMap->CheckClickTile(event.button.x, event.button.y);
                    if (event.button.button == SDL_BUTTON_LEFT)
                        //Checks if valid map position and sets tile
                        CurProject->LevelMap->CheckSetTile(event.button.x, event.button.y);
                    else if (event.button.button == SDL_BUTTON_RIGHT) {
                        //Checks if valid map position and selects tile
                        CurProject->LevelMap->CheckSelectTile(event.button.x, event.button.y);
                        CurProject->SelectionRect->SelInit(event.button.x, event.button.y);
                    }
		}
            }
            if (event.type == SDL_MOUSEBUTTONUP) 
            {
                if (event.button.button == SDL_BUTTON_RIGHT) {
		    if (CurProject != NULL)
		    {
                        //Checks if valid map position and selects tile
                        CurProject->LevelMap->CheckSelectTile(event.button.x, event.button.y);
                        CurProject->SelectionRect->SelFinalize(event.button.x, event.button.y);
		    }
                }
            }
            if (event.type == SDL_MOUSEMOTION)
            {
		if (CurProject != NULL)
		{
                    if (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
                        CurProject->LevelMap->CheckSetTile(event.motion.x, event.motion.y);
                    else {
                        CurProject->LevelMap->RefreshTileScreen(event.motion.x - event.motion.xrel, event.motion.y - event.motion.yrel, true);
                        CurProject->LevelMap->DrawSelectedTile(event.motion.x, event.motion.y);
                    }
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
		        if (CurProject != NULL)
			{
                            if (!CurProject->SelectionRect->isActive()) CurProject->LevelMap->CurSwapPriority();
                            else {
                                CurProject->SelectionRect->SwapPriority();
                                CurProject->LevelMap->DrawMap();
                                CurProject->SelectionRect->SelDrawRect();
                            }
			}
                        break;
                    case ',':
		        if (CurProject != NULL)
			{
                            if (!CurProject->SelectionRect->isActive()) CurProject->LevelMap->CurFlipX();
                            else {
                                CurProject->SelectionRect->FlipX();
                                CurProject->LevelMap->DrawMap();
                                CurProject->SelectionRect->SelDrawRect();
                            }
			}
                        break;
                    case '.':
		        if (CurProject != NULL)
			{
                            if (!CurProject->SelectionRect->isActive()) CurProject->LevelMap->CurFlipY();
                            else {
                                CurProject->SelectionRect->FlipY();
                                CurProject->LevelMap->DrawMap();
                                CurProject->SelectionRect->SelDrawRect();
                            }
			}
                        break;
                    case SDLK_RIGHT:
		        if (CurProject != NULL)
			{
                            CurProject->LevelMap->CurShiftRight();
                            CurProject->SelectionRect->Unselect();
			}
			break;
                    case SDLK_LEFT:
		        if (CurProject != NULL)
			{
                            CurProject->LevelMap->CurShiftLeft();
                            CurProject->SelectionRect->Unselect();
			}
			break;
                    case SDLK_DOWN:
		        if (CurProject != NULL)
			{
                            CurProject->LevelMap->CurShiftDown();
                            CurProject->SelectionRect->Unselect();
			}
			break;
                    case SDLK_UP:
		        if (CurProject != NULL)
			{
                            CurProject->LevelMap->CurShiftUp();
                            CurProject->SelectionRect->Unselect();
			}
			break;
                    case SDLK_DELETE:
		        if (CurProject != NULL)
			{
                            if (!CurProject->SelectionRect->isActive()) CurProject->LevelMap->ClearCurrentTile();
                            else {
                                CurProject->SelectionRect->clear();
                                CurProject->SelectionRect->AssignSection();
                                CurProject->LevelMap->DrawMap();
                            }
			}
                        break;
                    case SDLK_ESCAPE:
                        quit = true; break;
                    case SDLK_RETURN:
		        if (CurProject != NULL)
			{
                            CurProject->LevelMap->SetTileSelected();
                            CurProject->SelectionRect->Unselect();
			}
			break;
                    case '=':
                    case '´':
		        if (CurProject != NULL)
			{
                            CurProject->LevelMap->SelectTileCur();
                            CurProject->SelectionRect->Unselect();
			}
			break;
                    case SDLK_PAGEDOWN:
		        if (CurProject != NULL)
			{
                            if (!CurProject->SelectionRect->isActive()) CurProject->LevelMap->SelectedTileIncrID();
                            else {
                                CurProject->SelectionRect->IncrID();
                                CurProject->SelectionRect->AssignSection();
                                CurProject->LevelMap->DrawMap();
                            }
			}
                        break;
                    case SDLK_PAGEUP:
		        if (CurProject != NULL)
			{
                            if (!CurProject->SelectionRect->isActive()) CurProject->LevelMap->SelectedTileDecrID();
                            else {
                                CurProject->SelectionRect->DecrID();
                                CurProject->SelectionRect->AssignSection();
                                CurProject->LevelMap->DrawMap();
                            }
			}
                        break;
                    case SDLK_F1:
		        if (CurProject != NULL)
			{
                            CurProject->LevelMap->SetPalCurrent(0);
                            CurProject->GfxStuff->DrawSelector();
			}
			break;
                    case SDLK_F2:
		        if (CurProject != NULL)
			{
                            CurProject->LevelMap->SetPalCurrent(1);
                            CurProject->GfxStuff->DrawSelector();
			}
			break;
                    case SDLK_F3:
		        if (CurProject != NULL)
			{
                            CurProject->LevelMap->SetPalCurrent(2);
                            CurProject->GfxStuff->DrawSelector();
			}
			break;
                    case SDLK_F4:
		        if (CurProject != NULL)
			{
                            CurProject->LevelMap->SetPalCurrent(3);
                            CurProject->GfxStuff->DrawSelector();
			}
			break;
                    case SDLK_F5:
		        if (CurProject != NULL)
			{
                            CurProject->GfxStuff->ToggleHighPriority();
                            CurProject->LevelMap->DrawMap();
                            CurProject->SelectionRect->SelDrawRect();
			}
			break;
                    case SDLK_F6:
		        if (CurProject != NULL)
			{
                            CurProject->GfxStuff->ToggleLowPriority();
                            CurProject->LevelMap->DrawMap();
                            CurProject->SelectionRect->SelDrawRect();
			}
			break;
                    case SDLK_F9:
		        if (CurProject != NULL)
			{
                            CurProject->Save();
			}
			break;
                    case SDLK_F10: //redraw whole screen
		        if (CurProject != NULL)
			{
                            CurProject->Redraw();
			}
			break;
                    case SDLK_BACKSPACE:
		        if (CurProject != NULL)
			{
                            CurProject->LevelMap->CurShiftLeft();
                            CurProject->LevelMap->ClearCurrentTile(); 
                            CurProject->SelectionRect->Unselect();
			}
			break;
                    case '[':
                    case 'ü':
		        if (CurProject != NULL)
			{
                            CurProject->GfxStuff->DecScreenOffset();
                            CurProject->LevelMap->DrawMap();
                            CurProject->SelectionRect->SelDrawRect();
			}
			break;
                    case '\'':
                    case 'ä':
		        if (CurProject != NULL)
			{
                            CurProject->GfxStuff->IncScreenOffset();
                            CurProject->LevelMap->DrawMap();
                            CurProject->SelectionRect->SelDrawRect();
			}
			break;
                    case ';':
                    case 'ö':
		        if (CurProject != NULL)
			{
                            CurProject->GfxStuff->DecScreenXOffset();
                            CurProject->LevelMap->DrawMap();
                            CurProject->SelectionRect->SelDrawRect();
			}
			break;
                    case '\\':
                    case '#':
                    case '+':
                    case ']':
		        if (CurProject != NULL)
			{
                            CurProject->GfxStuff->IncScreenXOffset();
                            CurProject->LevelMap->DrawMap();
                            CurProject->SelectionRect->SelDrawRect();
			}
			break;
                    case SDLK_END:
		        if (CurProject != NULL)
			{
                            CurProject->GfxStuff->IncSelOffset();
                            CurProject->GfxStuff->DrawSelector();
			}
			break;
                    case SDLK_HOME:
		        if (CurProject != NULL)
			{
                            CurProject->GfxStuff->DecSelOffset();
                            CurProject->GfxStuff->DrawSelector();
			}
			break;
                    case SDLK_SPACE:
		        if (CurProject != NULL)
			{
                            CurProject->LevelMap->ClearCurrentTile();
                            CurProject->LevelMap->CurShiftRight();
			}
			break;
                    default:
                        if (event.key.keysym.sym >= 'a' && event.key.keysym.sym <= 'z') {
                            if (CtrlPress) {
                                switch(event.key.keysym.sym)
                                {
                                    case 'c':
				        if (CurProject != NULL)
					{
                                            if (CurProject->SelectionRect->isActive()) {
                                                delete CurProject->CopyRect;
                                                CurProject->CopyRect = new SelRect(CurProject->SelectionRect);
                                            }
					}
                                        break;
                                    case 'x':
				        if (CurProject != NULL)
					{
                                            if (CurProject->SelectionRect->isActive()) {
                                                delete CurProject->CopyRect;
                                                CurProject->CopyRect = new SelRect(CurProject->SelectionRect);
                                                CurProject->SelectionRect->clear();
                                                CurProject->SelectionRect->AssignSection();
                                                CurProject->LevelMap->DrawMap();
                                            }
					}
                                        break;
                                    case 'v':
				        if (CurProject != NULL)
					{
                                            CurProject->CopyRect->PasteSection();
                                            CurProject->LevelMap->DrawMap();
					}
                                        break;
                                    case 'a':
				        if (CurProject != NULL)
					{
                                            CurProject->SelectionRect->SelInit(0, 0);
                                            CurProject->SelectionRect->SelFinalize(CurProject->PrjData->map.xSize*8, CurProject->PrjData->map.ySize*8);
					}
                                        break;
                                }
                            } else {
				if (CurProject != NULL)
				{
                                    CurProject->LevelMap->SetCurrentTile(event.key.keysym.sym - 'a' + CurProject->GfxStuff->GetTileOffset() + CurProject->PrjData->letterOffset);
                                    CurProject->LevelMap->CurShiftRight();
                                    CurProject->SelectionRect->Unselect();
				}
                            }
                        }
                        else if (event.key.keysym.sym >= '0' && event.key.keysym.sym <= '9') {
			    if (CurProject != NULL)
			    {
                                CurProject->LevelMap->SetCurrentTile(event.key.keysym.sym - '0' + CurProject->GfxStuff->GetTileOffset() + CurProject->PrjData->numberOffset);
                                CurProject->LevelMap->CurShiftRight();
                                CurProject->SelectionRect->Unselect();
			    }
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

#ifdef _WIN32
            if (event.type == SDL_SYSWMEVENT)
	    {
		    WinAPI::HandleWindowsEvent(&event);
	    }
#endif

            MainScreen->ProcessDisplay();
        }
    }
    return 0;
}
