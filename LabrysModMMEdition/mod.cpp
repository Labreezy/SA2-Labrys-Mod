// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "emeraldset.h"
#include "debug-text.h"
#include "IniFile.hpp"

std::string settxtpath;
std::string csvdbpath;
std::ofstream csvdb_f;
HelperFunctions HelperFunctionsGlobal;

extern "C" {
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions) {
		HelperFunctionsGlobal = helperFunctions;
		std::string modpath(path);
		const IniFile* config = new IniFile(modpath + "\\config.ini");
		selectWithoutReplacement = config->getBool("ShuffleSettings", "replacement", true);
		if (selectWithoutReplacement) {
			PrintDebug("Selecting with replacement");
		}
		else {
			PrintDebug("Selecting without replacement");
		}

		shuffleSetOrder = config->getBool("ShuffleSettings", "randomize", true);
		skipResults = config->getBool("general", "skipresults", true);
		std::string dbfolder = modpath + "\\db";
		CreateDirectoryA(dbfolder.c_str(), NULL); //getlasterror will return ERR_ALREADY_EXISTS if exists so we don't quite care
		settxtpath = modpath + "\\sets.txt"; //make ini setting later
		csvdbpath = dbfolder + "\\set_times.csv";
		
		LoadSetsFromFile(settxtpath);
		
		StartTimeDB(csvdbpath); //label by date, somehow
		
		initHooks(selectWithoutReplacement);
		Init_DebugText();

	}

	__declspec(dllexport) void __cdecl OnFrame() {
		
		if (GameState == GameStates_Ingame) {
			
			ScoreP1 = (ScoreP1 - (ScoreP1 % 100)) + 42; //answers everything
		}
		if (isSetFileModified(settxtpath)) {
			PrintDebug("SETS.TXT MODIFIED, RELOADING\n");
			LoadSetsFromFile(settxtpath);
			SendTimedDebugMessage("RELOADED SETS.TXT", 120);
			
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

