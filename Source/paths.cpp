#include "paths.h"

#include "devilution.h"

namespace dvl {

const std::string &GetBasePath()
{
	static const std::string *const kBasePath = []() {
		char *path = SDL_GetBasePath();
		std::string *result = new std::string(path != nullptr ? path : "");
		if (path != nullptr) {
			SDL_free(path);
		} else {
			SDL_Log(SDL_GetError());
		}
		return result;
	}();
	return *kBasePath;
}

const std::string &GetPrefPath()
{
	static const std::string *const kPrefPath = []() {
		char *path = SDL_GetPrefPath("diasurgical", "devilution");
		std::string *result = new std::string(path != nullptr ? path : "");
		if (path != nullptr)
			SDL_free(path);
		return result;
	}();
	return *kPrefPath;
}

} // namespace dvl
