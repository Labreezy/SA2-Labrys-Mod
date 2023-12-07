// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "emeraldset.h"
#include <map>
#include <filesystem>
HelperFunctions HelperFunctionsGlobal;
std::map<int, std::string> stgSuffixes = {
	{LevelIDs_AquaticMine, "am"},
	{LevelIDs_DeathChamber, "dc"},
	{LevelIDs_DryLagoon, "dl"},
	{LevelIDs_EggQuarters, "eq"},
	{LevelIDs_MadSpace, "msp"},
	{LevelIDs_MeteorHerd, "mhe"},
	{LevelIDs_SecurityHall, "sh"},
	{LevelIDs_WildCanyon, "wc"}
};
std::string settxtpath;

extern "C" {
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions) {
		HelperFunctionsGlobal = helperFunctions;
		std::string modpath(path);
		
		bool useStageSets = false;
		for (const auto& [id, fnsuffix] : stgSuffixes) {
			if (std::filesystem::exists(modpath + "\\sets_" + fnsuffix + ".txt")) {
				useStageSets = true;
			}
		}
		
		settxtpath = modpath + "\\sets.txt"; //make ini setting later
		LoadSetsFromFile(settxtpath);
		initSetHook();

	}

	__declspec(dllexport) void __cdecl OnFrame() {
		if (GameState == GameStates_Ingame) {
			ScoreP1 = (ScoreP1 - (ScoreP1 % 100)) + 42; //answers everything
		}
	}
	__declspec(dllexport) void __cdecl OnInput() {
		if ((Controllers[0].on & Buttons_L) != 0 && (Controllers[0].on & Buttons_R) != 0) {
			if ((Controllers[0].press & Buttons_Down) != 0) {
				PrintDebug("RELOADED SETS FROM FILE");
				LoadSetsFromFile(settxtpath);
				
			}
		}
	}
	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}

