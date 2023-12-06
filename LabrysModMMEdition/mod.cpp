// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "emeraldset.h"

HelperFunctions HelperFunctionsGlobal;

extern "C" {
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions) {
		HelperFunctionsGlobal = helperFunctions;
		std::string modpath(path);
		
		
		
		std::string settxtpath = modpath + "\\sets.txt"; //make ini setting later
		LoadSetsFromFile(settxtpath);
		initSetHook();

	}

	__declspec(dllexport) void __cdecl OnFrame() {
		if (GameState == GameStates_Ingame) {
			ScoreP1 = (ScoreP1 - (ScoreP1 % 100)) + 42; //answers everything
		}
	}
	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}

