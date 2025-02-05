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

#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "libraries/imgui/backends/imgui_impl_sdl2.h"
#include "libraries/imgui/backends/imgui_impl_sdlrenderer2.h"

#include "Common.h"
#include "Tile.h"
#include "LevMap.h"
#include "Graphics.h"
#include "Screen.h"
#include "SelRect.h"
#include "Project.h"
#include "PrjHndl.h"
#include "Resource.h"

Screen MainScreen;
std::optional<Project> CurProject;

int main(int argc, char **argv)
{
	MainScreen.Clear();

	// For backwards-compatibility, we want project files that are drag-and-dropped
	// onto the executable to be automatically loaded when it starts.
	// First, we have to make sure the file's actually there.
	FILE *prjfile = (argc > 1) ? fopen(argv[1], "r") : nullptr;

	if (prjfile != nullptr)
	{
		fclose(prjfile);
		CurProject.emplace(argv[1]);

		// Process initial display
		CurProject->Redraw();
	}

	bool CtrlPress = false;

	//SDL_EnableKeyRepeat(400,SDL_DEFAULT_REPEAT_INTERVAL);

	//Main Loop including event Handling
	bool quit = false;
	while (!quit)
	{
		// Wait for an event
		if (SDL_WaitEvent(nullptr))
		{
			SDL_Event event;

			// Process all pending events
			while (SDL_PollEvent(&event))
			{
				MainScreen.ProcessEvent(event);

				switch (event.type)
				{
					case SDL_QUIT:
						quit = true;
						break;

					case SDL_KEYDOWN:
						//secondary cases usually for German keyboard layout support
						switch(event.key.keysym.sym)
						{
							case SDLK_RCTRL:
							case SDLK_LCTRL:
								CtrlPress = true;
								break;

							case '/':
							case '-':
								if (CurProject.has_value())
								{
									if (!CurProject->SelectionRect.isActive())
									{
										CurProject->LevelMap.CurSwapPriority();
									}
									else
									{
										CurProject->SelectionRect.SwapPriority();
										CurProject->LevelMap.DrawMap();
										CurProject->SelectionRect.SelDrawRect();
									}
								}
								break;

							case ',':
								if (CurProject.has_value())
								{
									if (!CurProject->SelectionRect.isActive())
									{
										CurProject->LevelMap.CurFlipX();
									}
									else
									{
										CurProject->SelectionRect.FlipX();
										CurProject->LevelMap.DrawMap();
										CurProject->SelectionRect.SelDrawRect();
									}
								}
								break;

							case '.':
								if (CurProject.has_value())
								{
									if (!CurProject->SelectionRect.isActive())
									{
										CurProject->LevelMap.CurFlipY();
									}
									else
									{
										CurProject->SelectionRect.FlipY();
										CurProject->LevelMap.DrawMap();
										CurProject->SelectionRect.SelDrawRect();
									}
								}
								break;

							case SDLK_RIGHT:
								if (CurProject.has_value())
								{
									CurProject->LevelMap.CurShiftRight();
									CurProject->SelectionRect.Unselect();
								}
								break;

							case SDLK_LEFT:
								if (CurProject.has_value())
								{
									CurProject->LevelMap.CurShiftLeft();
									CurProject->SelectionRect.Unselect();
								}
								break;

							case SDLK_DOWN:
								if (CurProject.has_value())
								{
									CurProject->LevelMap.CurShiftDown();
									CurProject->SelectionRect.Unselect();
								}
								break;

							case SDLK_UP:
								if (CurProject.has_value())
								{
									CurProject->LevelMap.CurShiftUp();
									CurProject->SelectionRect.Unselect();
								}
								break;

							case SDLK_DELETE:
								if (CurProject.has_value())
								{
									if (!CurProject->SelectionRect.isActive())
									{
										CurProject->LevelMap.ClearCurrentTile();
									}
									else
									{
										CurProject->SelectionRect.clear();
										CurProject->SelectionRect.AssignSection();
										CurProject->LevelMap.DrawMap();
									}
								}
								break;

							case SDLK_ESCAPE:
								quit = true;
								break;

							case SDLK_RETURN:
								if (CurProject.has_value())
								{
									CurProject->LevelMap.SetTileSelected();
									CurProject->SelectionRect.Unselect();
								}
								break;

							case '=':
							case '\xB4': // �
								if (CurProject.has_value())
								{
									CurProject->LevelMap.SelectTileCur();
									CurProject->SelectionRect.Unselect();
								}
								break;

							case SDLK_PAGEDOWN:
								if (CurProject.has_value())
								{
									if (!CurProject->SelectionRect.isActive())
									{
										CurProject->LevelMap.SelectedTileIncrID();
									}
									else
									{
										CurProject->SelectionRect.IncrID();
										CurProject->SelectionRect.AssignSection();
										CurProject->LevelMap.DrawMap();
									}
								}
								break;

							case SDLK_PAGEUP:
								if (CurProject.has_value())
								{
									if (!CurProject->SelectionRect.isActive())
									{
										CurProject->LevelMap.SelectedTileDecrID();
									}
									else
									{
										CurProject->SelectionRect.DecrID();
										CurProject->SelectionRect.AssignSection();
										CurProject->LevelMap.DrawMap();
									}
								}
								break;

							case SDLK_F1:
								if (CurProject.has_value())
								{
									CurProject->LevelMap.SetPalCurrent(0);
									CurProject->GfxStuff.DrawSelector();
								}
								break;

							case SDLK_F2:
								if (CurProject.has_value())
								{
									CurProject->LevelMap.SetPalCurrent(1);
									CurProject->GfxStuff.DrawSelector();
								}
								break;

							case SDLK_F3:
								if (CurProject.has_value())
								{
									CurProject->LevelMap.SetPalCurrent(2);
									CurProject->GfxStuff.DrawSelector();
								}
								break;

							case SDLK_F4:
								if (CurProject.has_value())
								{
									CurProject->LevelMap.SetPalCurrent(3);
									CurProject->GfxStuff.DrawSelector();
								}
								break;

							case SDLK_F5:
								if (CurProject.has_value())
								{
									CurProject->GfxStuff.ToggleHighPriority();
									CurProject->LevelMap.DrawMap();
									CurProject->SelectionRect.SelDrawRect();
								}
								break;

							case SDLK_F6:
								if (CurProject.has_value())
								{
									CurProject->GfxStuff.ToggleLowPriority();
									CurProject->LevelMap.DrawMap();
									CurProject->SelectionRect.SelDrawRect();
								}
								break;

							case SDLK_F9:
								if (CurProject.has_value())
									CurProject->Save();
								break;

							case SDLK_F10: //redraw whole screen
								if (CurProject.has_value())
									CurProject->Redraw();
								break;

							case SDLK_BACKSPACE:
								if (CurProject.has_value())
								{
									CurProject->LevelMap.CurShiftLeft();
									CurProject->LevelMap.ClearCurrentTile();
									CurProject->SelectionRect.Unselect();
								}
								break;

							case '[':
							case '\xFC': // �
								if (CurProject.has_value())
								{
									CurProject->GfxStuff.DecScreenOffset();
									CurProject->LevelMap.DrawMap();
									CurProject->SelectionRect.SelDrawRect();
								}
								break;

							case '\'':
							case '\xE4': // �
								if (CurProject.has_value())
								{
									CurProject->GfxStuff.IncScreenOffset();
									CurProject->LevelMap.DrawMap();
									CurProject->SelectionRect.SelDrawRect();
								}
								break;

							case ';':
							case '\xF6': // �
								if (CurProject.has_value())
								{
									CurProject->GfxStuff.DecScreenXOffset();
									CurProject->LevelMap.DrawMap();
									CurProject->SelectionRect.SelDrawRect();
								}
								break;

							case '\\':
							case '#':
							case '+':
							case ']':
								if (CurProject.has_value())
								{
									CurProject->GfxStuff.IncScreenXOffset();
									CurProject->LevelMap.DrawMap();
									CurProject->SelectionRect.SelDrawRect();
								}
								break;

							case SDLK_END:
								if (CurProject.has_value())
								{
									CurProject->GfxStuff.IncSelOffset();
									CurProject->GfxStuff.DrawSelector();
								}
								break;

							case SDLK_HOME:
								if (CurProject.has_value())
								{
									CurProject->GfxStuff.DecSelOffset();
									CurProject->GfxStuff.DrawSelector();
								}
								break;

							case SDLK_SPACE:
								if (CurProject.has_value())
								{
									CurProject->LevelMap.ClearCurrentTile();
									CurProject->LevelMap.CurShiftRight();
								}
								break;

							default:
								if (event.key.keysym.sym >= 'a' && event.key.keysym.sym <= 'z')
								{
									if (CtrlPress)
									{
										switch (event.key.keysym.sym)
										{
											case 'c':
												if (CurProject.has_value())
												{
													if (CurProject->SelectionRect.isActive())
													{
														CurProject->CopyRect.emplace(CurProject->SelectionRect);
													}
												}
												break;

											case 'x':
												if (CurProject.has_value())
												{
													if (CurProject->SelectionRect.isActive())
													{
														CurProject->CopyRect.emplace(CurProject->SelectionRect);
														CurProject->SelectionRect.clear();
														CurProject->SelectionRect.AssignSection();
														CurProject->LevelMap.DrawMap();
													}
												}
												break;

											case 'v':
												if (CurProject.has_value())
												{
													CurProject->CopyRect->PasteSection();
													CurProject->LevelMap.DrawMap();
												}
												break;

											case 'a':
												if (CurProject.has_value())
												{
													CurProject->SelectionRect.SelInit(0, 0);
													CurProject->SelectionRect.SelFinalize(CurProject->PrjData.map.xSize*8, CurProject->PrjData.map.ySize*8);
												}
												break;
										}
									}
									else
									{
										if (CurProject.has_value())
										{
											CurProject->LevelMap.SetCurrentTile(event.key.keysym.sym - 'a' + CurProject->GfxStuff.GetTileOffset() + CurProject->PrjData.letterOffset);
											CurProject->LevelMap.CurShiftRight();
											CurProject->SelectionRect.Unselect();
										}
									}
								}
								else if (event.key.keysym.sym >= '0' && event.key.keysym.sym <= '9')
								{
									if (CurProject.has_value())
									{
										CurProject->LevelMap.SetCurrentTile(event.key.keysym.sym - '0' + CurProject->GfxStuff.GetTileOffset() + CurProject->PrjData.numberOffset);
										CurProject->LevelMap.CurShiftRight();
										CurProject->SelectionRect.Unselect();
									}
								}

								break;
						}

						break;

					case SDL_KEYUP:
						switch(event.key.keysym.sym)
						{
							case SDLK_RCTRL:
							case SDLK_LCTRL:
								CtrlPress = false;
								break;
						}
						break;

					case SDL_WINDOWEVENT:
						switch (event.window.event)
						{
							case SDL_WINDOWEVENT_RESIZED:
							case SDL_WINDOWEVENT_SIZE_CHANGED:
								MainScreen.WindowResized(event.window.data1, event.window.data2);
								break;
						}
						break;

					case SDL_RENDER_TARGETS_RESET:
						MainScreen.Clear();

						if (CurProject.has_value())
							CurProject->Redraw();

						break;
				}
			}

			// Now that we're finished processing all pending events,
			// we finally have some spare time to update the window
			MainScreen.ProcessDisplay();

			// Now that we're done, loop back and idle until another event occurs
		}
	}
	return 0;
}
