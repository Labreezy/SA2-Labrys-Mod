// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "emeraldset.h"
#include "pieceutils.h"
#include "debug-text.h"
#include "./IniFile.hpp"

std::string settxtpath;
std::string csvdbpath;

std::ofstream csvdb_f;
bool shuffleSetOrder = false;
HelperFunctions HelperFunctionsGlobal;
bool DEATH_STRAT_MODE = false;
bool minsets = false;
extern "C" {
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions) {
		HelperFunctionsGlobal = helperFunctions;
		std::string modpath(path);
		const IniFile* config = new IniFile(modpath + "\\config.ini");
		DEATH_STRAT_MODE = config->getBool("PostDeathSettings", "enabled", false);
		minsets = config->getBool("GeneralSettings", "minsets", false);
		std::string safecolorstr = config->getString("GeneralSettings", "safecolor", "yellow");
		shuffleSetOrder = config->getBool("ShuffleSettings", "randomize", true);
		PrintDebug("SET %s SAFES", safecolorstr.c_str());
		if (safecolorstr == "yellow") {
			safeColorIndex = 1;
		}
		else if (safecolorstr == "red") {
			safeColorIndex = 2;
		}
		else {
			safeColorIndex = 0;
		}

		if (!DEATH_STRAT_MODE && !minsets) {
			selectWithoutReplacement = config->getBool("ShuffleSettings", "replacement", true);
			if (selectWithoutReplacement) {
				PrintDebug("Selecting with replacement");
			}
			else {
				PrintDebug("Selecting without replacement");
			}

			
			PrintDebug("GOT ANOTHER THING");
			//std::string dbfolder = modpath + "\\db";
			//CreateDirectoryA(dbfolder.c_str(), NULL); //getlasterror will return ERR_ALREADY_EXISTS if exists so we don't quite care
			settxtpath = modpath + "\\sets.txt"; //make ini setting later
			//csvdbpath = dbfolder + "\\set_times.csv";
		}

		if (DEATH_STRAT_MODE) {
			PrintDebug("DEATH STRAT MODE ON");
			std::string pieceGroup = config->getString("PostDeathSettings", "group");
			initPostDeathHook(pieceGroup);
		}
		else if (minsets) {
			PrintDebug("MINIMUM SET MODE ON");
			initHooks(shuffleSetOrder, selectWithoutReplacement, minsets);
		}
		else {
			PrintDebug("LOADING FROM FILE");
			LoadSetsFromFile(settxtpath);

			//StartTimeDB(csvdbpath); //label by date, somehow

			initHooks(shuffleSetOrder, selectWithoutReplacement, minsets);
		}
		Init_DebugText();

	}

	__declspec(dllexport) void __cdecl OnFrame() {
		
		if (GameState == GameStates_Ingame) {
			
			ScoreP1 = (ScoreP1 - (ScoreP1 % 10)) + 7; //RIP 42 but score attacks gotta see that number
		}
		if (!DEATH_STRAT_MODE && !minsets){
			if (isSetFileModified(settxtpath)) {
				PrintDebug("SETS.TXT MODIFIED, RELOADING\n");
				LoadSetsFromFile(settxtpath);
				SendTimedDebugMessage("RELOADED SETS.TXT", 120);
			
			}
		}
		DisplayTimed_DebugMessage_OnFrames();
		
		

	}
	__declspec(dllexport) void __cdecl OnInput() {
		if (Controllers[0].on & Buttons_L && Controllers[0].on & Buttons_R) {
			if (Controllers[0].press & Buttons_Up) {
				SendTimedDebugMessage("OUPUT TIME DATA TO DB (WIP)", 120);
			}
		}
	}

	__declspec(dllexport) void __cdecl OnExit() {
	
		ildb_outf->close();
	}
	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}

