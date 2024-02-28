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

#include "Screen.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <algorithm>

#include "SDL.h"

#include "libraries/imgui/backends/imgui_impl_sdl2.h"
#include "libraries/imgui/backends/imgui_impl_sdlrenderer2.h"

#include "Common.h"
#include "PrjHndl.h"

#include "noto-sans-regular.h"

Screen::Screen(void) : file_utilities([this](const char* const format, std::va_list args){ShowInternalError(format, args);})
{
	SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING, "1");

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		this->ShowInternalError("Unable to init SDL video\n\n%s", SDL_GetError());

	atexit(SDL_Quit); // TODO: Do this in the destructor?

	this->window = SDL_CreateWindow("Captain PlaneEd v1.2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	if (this->window == nullptr)
		this->ShowInternalError("Unable to init SDL Window\n\n%s", SDL_GetError());

	this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);
	if (this->renderer == nullptr)
		this->ShowInternalError("Unable to init SDL Renderer\n\n%s", SDL_GetError());

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (this->texture == nullptr)
		this->ShowInternalError("Unable to init screen SDL Texture\n\n%s", SDL_GetError());

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	this->upscaled_texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (this->upscaled_texture == nullptr)
		this->ShowInternalError("Unable to init upscaled screen SDL Texture\n\n%s", SDL_GetError());


	this->background_colour.r = 0;
	this->background_colour.g = 0;
	this->background_colour.b = 0;
	this->background_colour.a = 0xFF;

	this->display_changed = false;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigWindowsMoveFromTitleBarOnly = true; // This prevents drag-drawing from moving the window.
	io.IniFilename = nullptr; // Disable that annoying INI file.

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(this->window, this->renderer);
	ImGui_ImplSDLRenderer2_Init(this->renderer);

	ImGuiStyle& style = ImGui::GetStyle();
	dpi_scale = GetDPIScale();
	style_backup = style;
	style.ScaleAllSizes(dpi_scale);
	ReloadFonts();
}

Screen::~Screen(void)
{
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void Screen::MarkDisplayChanged(void)
{
	this->display_changed = true;
}

void Screen::ProcessDisplay(void)
{
	if (this->display_changed)
	{
		// Upscale the framebuffer.
		SDL_SetRenderTarget(this->renderer, this->upscaled_texture);
		SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr);
	}

	// Handle dynamic DPI support
	const auto new_dpi = GetDPIScale();

	if (dpi_scale != new_dpi)
	{
		dpi_scale = new_dpi;

		ImGuiStyle& style = ImGui::GetStyle();
		style = style_backup;
		style.ScaleAllSizes(dpi_scale);
		ReloadFonts();
	}

	// Switch to the real framebuffer now, so Dear ImGui doesn't freak-out.
	SDL_SetRenderTarget(this->renderer, nullptr);

	// Start the Dear ImGui frame
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	const ImGuiViewport *viewport = ImGui::GetMainViewport();

	// Maximise the window if needed.
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	// Prevent the window from getting too small or we'll get division by zero errors later on.
	ImGui::SetNextWindowSizeConstraints(ImVec2(100.0f, 100.0f), ImVec2(FLT_MAX, FLT_MAX)); // Width > 100, Height > 100

	// Tweak the style so that the display fill the window.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	const bool not_collapsed = ImGui::Begin("Display", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar);
	ImGui::PopStyleVar();

	if (not_collapsed)
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open"))
				{
					file_utilities.CreateOpenFileDialog(window, "Open project file", [this](const char *path){
						if (!file_utilities.FileExists(path))
							return false;

						delete CurProject;
						CurProject = new Project(path);

						//Process initial display
						Clear();
						CurProject->Redraw();
						return true;
					});
				}

				if (ImGui::MenuItem("Save", nullptr, false, CurProject != nullptr))
					CurProject->Save();

				if (ImGui::MenuItem("Close", nullptr, false, CurProject != nullptr))
				{
					delete CurProject;
					CurProject = nullptr;	// Deleting an object does not null this pointer, so we have to do it ourselves
					Clear();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
					exit(1);


				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				float colour_channels[3];

				colour_channels[0] = background_colour.r / 255.0f;
				colour_channels[1] = background_colour.g / 255.0f;
				colour_channels[2] = background_colour.b / 255.0f;

				if (ImGui::ColorEdit3("Background colour", colour_channels, ImGuiColorEditFlags_NoInputs))
				{
					background_colour.r = colour_channels[0] * 255.0f;
					background_colour.g = colour_channels[1] * 255.0f;
					background_colour.b = colour_channels[2] * 255.0f;

					MainScreen->Clear();

					if (CurProject != NULL)
						CurProject->Redraw();
				}

				ImGui::Separator();

				for (int i = 0; i < 4; ++i)
				{
					char string_buffer[] = "Palette line 1";
					string_buffer[sizeof(string_buffer) - 2] = '1' + i;

					const auto current_palette_line = CurProject == nullptr ? 0 : CurProject->GfxStuff->GetCurrentPal();

					if (ImGui::MenuItem(string_buffer, nullptr, current_palette_line == i, CurProject != nullptr))
					{
						CurProject->LevelMap->SetPalCurrent(i);
						CurProject->GfxStuff->DrawSelector();
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		// The render buffer may have been changed, so put it back.
		SDL_SetRenderTarget(this->renderer, nullptr);

		// We need this block of logic to be outside of the below condition so that the editor
		// has keyboard focus on startup without needing the window to be clicked first.
		// Weird behaviour - I know.
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
		const ImVec2 size_of_display_region = ImGui::GetContentRegionAvail();

		// Create an invisible button which detects when input is intended for the editor.
		// We do this cursor stuff so that the framebuffer is drawn over the button.
		const ImVec2 cursor = ImGui::GetCursorPos();
		ImGui::InvisibleButton("Magical editor focus detector", size_of_display_region, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

		if (CurProject != nullptr)
		{
			const auto &io = ImGui::GetIO();
			const ImVec2 origin(canvas_p0.x, canvas_p0.y); // Lock scrolled origin
			const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);
			const Sint32 x = (mouse_pos_in_canvas.x - screen_x) / screen_scale;
			const Sint32 y = (mouse_pos_in_canvas.y - screen_y) / screen_scale;

			if (ImGui::IsItemHovered())
			{
				if (left_mouse_button_down)
					CurProject->LevelMap->CheckSetTile(x, y);

				CurProject->LevelMap->RefreshTileScreen(previous_mouse_x, previous_mouse_y, true);
				CurProject->LevelMap->DrawSelectedTile(x, y);

				previous_mouse_x = x;
				previous_mouse_y = y;
			}

			if (ImGui::IsItemActive())
			{
				const bool left_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
				const bool right_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Right);

				if (left_clicked || right_clicked)
				{
					CurProject->SelectionRect->Unselect();

					//Checks if within selector bounds and selects tile
					CurProject->LevelMap->CheckClickTile(x, y);

					if (left_clicked)
					{
						//Checks if valid map position and sets tile
						CurProject->LevelMap->CheckSetTile(x, y);
						left_mouse_button_down = true;
					}
					else if (right_clicked)
					{
						//Checks if valid map position and selects tile
						CurProject->LevelMap->CheckSelectTile(x, y);
						CurProject->SelectionRect->SelInit(x, y);
						right_mouse_button_down = true;
					}
				}
			}

			if (left_mouse_button_down && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
				left_mouse_button_down = false;

			if (right_mouse_button_down && !ImGui::IsMouseDown(ImGuiMouseButton_Right))
			{
				right_mouse_button_down = false;

				//Checks if valid map position and selects tile
				CurProject->LevelMap->CheckSelectTile(x, y);
				CurProject->SelectionRect->SelFinalize(x, y);
			}
		}

		// The render buffer may have been changed, so put it back.
		SDL_SetRenderTarget(this->renderer, nullptr);

		ImGui::SetCursorPos(cursor);

		const unsigned int work_width = static_cast<unsigned int>(size_of_display_region.x);
		const unsigned int work_height = static_cast<unsigned int>(size_of_display_region.y);

		const unsigned int destination_width = SCREEN_WIDTH;
		const unsigned int destination_height = SCREEN_HEIGHT;

		unsigned int destination_width_scaled;
		unsigned int destination_height_scaled;

		// Correct the aspect ratio of the rendered frame.
		// (256x224 and 320x240 should be the same width, but 320x224 and 320x240 should be different heights - this matches the behaviour of a real Mega Drive).
		if (work_width > work_height * destination_width / destination_height)
		{
			destination_width_scaled = work_height * destination_width / destination_height;
			destination_height_scaled = work_height;
			screen_scale = static_cast<float>(destination_height_scaled) / destination_height;
		}
		else
		{
			destination_width_scaled = work_width;
			destination_height_scaled = work_width * destination_height / destination_width;
			screen_scale = static_cast<float>(destination_width_scaled) / destination_width;
		}

		// Center the framebuffer in the available region.
		screen_x = (static_cast<int>(size_of_display_region.x) - destination_width_scaled) / 2;
		screen_y = (static_cast<int>(size_of_display_region.y) - destination_height_scaled) / 2;
		ImGui::SetCursorPosX(static_cast<float>(static_cast<int>(ImGui::GetCursorPosX()) + screen_x));
		ImGui::SetCursorPosY(static_cast<float>(static_cast<int>(ImGui::GetCursorPosY()) + screen_y));

		// Draw the upscaled framebuffer in the window.
		ImGui::Image(this->upscaled_texture != nullptr ? this->upscaled_texture : this->texture, ImVec2(static_cast<float>(destination_width_scaled), static_cast<float>(destination_height_scaled)));
	}

	ImGui::End();

	file_utilities.DisplayFileDialog();

	// Draw the frame.
	SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(this->renderer);
	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(this->renderer);
	SDL_SetRenderTarget(this->renderer, this->texture);
}

void Screen::Clear(void)
{
	this->MarkDisplayChanged();

	SDL_SetRenderTarget(this->renderer, this->texture);

	SDL_SetRenderDrawColor(this->renderer, this->background_colour.r, this->background_colour.g, this->background_colour.b, this->background_colour.a);
	SDL_RenderFillRect(this->renderer, nullptr);
}

void Screen::WindowResized(int width, int height)
{
	this->MarkDisplayChanged();

	static unsigned int upscale_factor;

	unsigned int new_upscale_factor = std::max(1, std::min((width + SCREEN_WIDTH / 2) / SCREEN_WIDTH, (height + SCREEN_HEIGHT / 2) / SCREEN_HEIGHT));

	if (new_upscale_factor != upscale_factor)
	{
		upscale_factor = new_upscale_factor;

		SDL_DestroyTexture(this->upscaled_texture);

		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		this->upscaled_texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH * upscale_factor, SCREEN_HEIGHT * upscale_factor);
	}
}

void Screen::ShowInformation(const char *message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Information", message, this->window);
}

void Screen::ShowInformation(const char *message_part1, const char *message_part2)
{
	// TODO: Use SDL_asprintf.
	char *whole_message = new char[strlen(message_part1)+strlen(message_part2)+1];
	sprintf(whole_message, "%s%s", message_part1, message_part2);
	this->ShowInformation(whole_message);
	delete[] whole_message;
}

void Screen::ShowWarning(const char *message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", message, this->window);
}

void Screen::ShowError(const char *message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, this->window);
	exit(1);
}

void Screen::ShowInternalError(const char *format, std::va_list args)
{
	char *message_buffer;
	if (SDL_vasprintf(&message_buffer, format, args) != -1)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Internal Error", message_buffer, this->window);
		SDL_free(message_buffer);
	}
	exit(1);
}

void Screen::ShowInternalError(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	ShowInternalError(format, args);
	va_end(args);
}

void Screen::ProcessEvent(const SDL_Event &event)
{
	SDL_SetRenderTarget(renderer, nullptr);
	ImGui_ImplSDL2_ProcessEvent(&event);
}

// TODO: Cache this or whatever.
float Screen::GetDPIScale(void) const
{
	const auto texture = SDL_GetRenderTarget(this->renderer);
	SDL_SetRenderTarget(this->renderer, nullptr);

	int window_width, renderer_width;
	SDL_GetWindowSize(window, &window_width, nullptr);
	SDL_GetRendererOutputSize(renderer, &renderer_width, nullptr);

	SDL_SetRenderTarget(this->renderer, texture);

	const float dpi_scale = static_cast<float>(renderer_width) / std::max(1, window_width); // Prevent a division by 0.

	// Prevent any insanity if we somehow get bad values.
	return std::max(1.0f, dpi_scale);
}

static constexpr float UNSCALED_FONT_SIZE = 16.0f;

void Screen::ReloadFonts(void)
{
	ImGuiIO &io = ImGui::GetIO();

	io.Fonts->Clear();
	ImGui_ImplSDLRenderer2_DestroyFontsTexture();

	const unsigned int font_size = UNSCALED_FONT_SIZE * dpi_scale;

	ImFontConfig font_cfg = ImFontConfig();
	SDL_snprintf(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "Noto Sans Regular, %upx", font_size);
	io.Fonts->AddFontFromMemoryCompressedTTF(noto_sans_regular_compressed_data, noto_sans_regular_compressed_size, static_cast<float>(font_size), &font_cfg);
}
