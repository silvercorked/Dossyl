
#include "Common.h"

#include "CommonHeaders.h"

#include "..\Engine\Components\Script.h"

#ifndef WIN32_MEAN_AND_LEAN
#define WIN32_MEAN_AND_LEAN
#endif
#include <Windows.h>

using namespace Dossyl;

namespace {
	HMODULE gameCodeDll{ nullptr };
	using _getScriptCreator = Dossyl::Script::Detail::script_creator(*)(size_t);
	_getScriptCreator getScriptCreator{ nullptr };
	using _getScriptNames = LPSAFEARRAY(*)();
	_getScriptNames getScriptNames{ nullptr };
}

EDITOR_INTERFACE auto LoadGameCodeDll(const char* dll_path) -> u32 {
	if (gameCodeDll)
		return FALSE;
	gameCodeDll = LoadLibraryA(dll_path); // load dll as ascii
	assert(gameCodeDll);

	// i think reinterpret cast is lowest power. static_cast didnt work. unsure tho.
	getScriptCreator = reinterpret_cast<_getScriptCreator>(GetProcAddress(gameCodeDll, "getScriptCreator"));
	getScriptNames = reinterpret_cast<_getScriptNames>(GetProcAddress(gameCodeDll, "getScriptNames"));

	return (gameCodeDll && getScriptCreator && getScriptNames) ? TRUE : FALSE;
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

EDITOR_INTERFACE auto GetScriptCreator(const char* name) -> Script::Detail::script_creator {
	return (gameCodeDll && getScriptCreator)
		? getScriptCreator(Script::Detail::string_hash()(name))
		: nullptr;
}

EDITOR_INTERFACE auto GetScriptNAmes() -> LPSAFEARRAY {
	return (gameCodeDll && getScriptNames)
		? getScriptNames()
		: nullptr;
}
