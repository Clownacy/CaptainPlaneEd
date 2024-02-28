#ifndef FILE_UTILITIES_H
#define FILE_UTILITIES_H

#if defined(__unix__) && !defined(__EMSCRIPTEN__)
#include <unistd.h>

#if defined(_POSIX_VERSION) && _POSIX_VERSION >= 200112L
#define FILE_PICKER_POSIX
#endif
#endif

#if defined(_WIN32) || defined(FILE_PICKER_POSIX)
#define FILE_PICKER_HAS_NATIVE_FILE_DIALOGS
#endif

#include <cstdarg>
#include <cstddef>
#include <functional>
#include <vector>

#include "clowncommon/clowncommon.h"

#include "sdl-wrapper.h"

class FileUtilities
{
public:
	using SaveFileInnerCallback = std::function<bool(const void *data, std::size_t data_size)>;

private:
	using PopupCallback = std::function<bool(const char *path)>;
	using LoadFileCallback = std::function<bool(const char *path, SDL::RWops &file)>;
	using SaveFileCallback = std::function<bool(const SaveFileInnerCallback &save_file)>;
	using DebugCallback = std::function<void(const char* format, std::va_list args)>;

	int text_buffer_size;
	char *text_buffer = nullptr;

	DebugCallback debug_callback;
	const char *active_file_picker_popup = nullptr;
	PopupCallback popup_callback;
	bool is_save_dialog = false;

	void CreateFileDialog(SDL_Window *window, const char *title, const PopupCallback &callback, bool save);
	CC_ATTRIBUTE_PRINTF(2, 3) void DebugLog(const char* format, ...);

public:
#ifdef FILE_PICKER_POSIX
	char *last_file_dialog_directory = nullptr;
	bool prefer_kdialog = false;
#endif

#ifdef FILE_PICKER_HAS_NATIVE_FILE_DIALOGS
	bool use_native_file_dialogs = true;
#endif

	FileUtilities(const DebugCallback &debug_callback) : debug_callback(debug_callback) {}
#ifdef FILE_PICKER_POSIX
	~FileUtilities() {SDL_free(last_file_dialog_directory);}
#endif
	void CreateOpenFileDialog(SDL_Window *window, const char *title, const PopupCallback &callback);
	void CreateSaveFileDialog(SDL_Window *window, const char *title, const PopupCallback &callback);
	void DisplayFileDialog() {char* dummy = nullptr; DisplayFileDialog(dummy);}
	void DisplayFileDialog(char *&drag_and_drop_filename);
	bool IsDialogOpen() const { return active_file_picker_popup != nullptr; }

	bool FileExists(const char *filename);
	bool LoadFileToBuffer(std::vector<unsigned char> &file_buffer, const char *filename);
	bool LoadFileToBuffer(std::vector<unsigned char> &file_buffer, const SDL::RWops &file);

	void LoadFile(SDL_Window *window, const char *title, const LoadFileCallback &callback);
	void SaveFile(SDL_Window *window, const char *title, const SaveFileCallback &callback);
};

#endif /* FILE_UTILITIES_H */
