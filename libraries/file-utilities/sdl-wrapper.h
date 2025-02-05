#ifndef SDL_WRAPPER_H
#define SDL_WRAPPER_H

#include <memory>

#include "SDL.h"

#define SDL_WRAPPER_MAKE_RAII_POINTER(NAME, TYPE, DELETER) \
struct NAME##Deleter \
{ \
	void operator()(TYPE* const pointer) { DELETER(pointer); } \
}; \
 \
class NAME : public std::unique_ptr<TYPE, NAME##Deleter> \
{ \
private: \
	using Base = std::unique_ptr<TYPE, NAME##Deleter>; \
 \
public: \
	using Base::unique_ptr; \
	NAME(TYPE* const pointer) : Base(pointer) {} \
	operator const TYPE*() const { return get(); } \
	operator TYPE*() { return get(); } \
}

namespace SDL {

SDL_WRAPPER_MAKE_RAII_POINTER(Window,   SDL_Window,   SDL_DestroyWindow  );
SDL_WRAPPER_MAKE_RAII_POINTER(Renderer, SDL_Renderer, SDL_DestroyRenderer);
SDL_WRAPPER_MAKE_RAII_POINTER(Texture,  SDL_Texture,  SDL_DestroyTexture );
SDL_WRAPPER_MAKE_RAII_POINTER(RWops,    SDL_RWops,    SDL_RWclose        );

template<auto Function, typename... Args>
auto PathFunction(const char* const path, Args &&...args)
{
	return Function(path, std::forward<Args>(args)...);
};

template<auto Function, typename... Args>
auto PathFunction(const std::string &path, Args &&...args)
{
	return Function(path.c_str(), std::forward<Args>(args)...);
};

template<auto Function, typename... Args>
auto PathFunction(const std::filesystem::path &path, Args &&...args)
{
	return Function(reinterpret_cast<const char*>(path.u8string().c_str()), std::forward<Args>(args)...);
};

template<typename T>
RWops RWFromFile(const T &path, const char* const mode) { return RWops(PathFunction<SDL_RWFromFile>(path, mode)); }

}

#endif /* SDL_WRAPPER_H */
