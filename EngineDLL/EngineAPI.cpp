
#include "Common.h"

#include "CommonHeaders.h"

#ifndef WIN32_MEAN_AND_LEAN
#define WIN32_MEAN_AND_LEAN
#endif
#include <Windows.h>

using namespace Dossyl;

namespace {
	HMODULE gameCodeDll{ nullptr };
}

EDITOR_INTERFACE auto LoadGameCodeDll(const char* dll_path) -> u32 {
	if (gameCodeDll)
		return FALSE;
	gameCodeDll = LoadLibraryA(dll_path); // load dll as ascii
	assert(gameCodeDll);
	return gameCodeDll ? TRUE : FALSE;
}

EDITOR_INTERFACE auto UnloadGameCodeDll() -> u32 {
	if (!gameCodeDll)
		return FALSE;
	assert(gameCodeDll);
	int result{ FreeLibrary(gameCodeDll) };
	assert(result);
	gameCodeDll = nullptr;
	return TRUE;
}
