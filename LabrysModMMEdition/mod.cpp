// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"


HelperFunctions HelperFunctionsGlobal;

std::string safeColor;
std::string setPath;

extern "C" {
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions) {
		HelperFunctionsGlobal = helperFunctions;
		const IniFile* config = new IniFile(std::string(path) + "\\config.ini");
		safeColor = config->getString("General", "safeColor", "Blue");
		setPath = config->getString("General", "setPath", "sets.txt");
		

	}

	__declspec(dllexport) void __cdecl OnFrame() {
		if (GameState == GameStates_Ingame) {
			ScoreP1 = (ScoreP1 - (ScoreP1 % 100)) + 69; //nice
			if (Life_Count != 99) {
				Life_Count = 99;
			}
		}
	}
	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}

