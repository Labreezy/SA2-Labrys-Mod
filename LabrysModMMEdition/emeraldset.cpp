#include "pch.h"
#include "emeraldset.h"
#include "asmutils.h"
#include "mod.h"
#include "debug-text.h"
#include <random>
#include <iterator>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>

int hookEntryPt = 0x7380BF;
int resultsEntryPt = 0x43e80e;
DWORD hookExit = hookEntryPt + 6;
DWORD resultsSkipExit = resultsEntryPt + 5;
DWORD noScoreExit = 0x451535;
DWORD run_start_const = 0x174aff9;
DWORD hookSafeColorPt = 0x635449;
DWORD hookSafeColorExit = hookSafeColorPt + 6;
int curr_set_idx = 0;
bool hasSetSafes = false;
int lastLevel = -1;
bool useMinSets = false;
bool shuffleSets = false;
std::filesystem::file_time_type last_setf_write;
std::vector<int> setIDs;
std::vector<int> setIDsCopy;
static std::random_device rd;
static std::mt19937 gen;
int safeSwitchArrayBase = 0x1DE9461;
std::vector<int> min_sets_wc = { 174,785,258,783,959,774,563,395,413,70,87,183,842,809,722,341,798,1003,827,473,992,334,255,972,485,399,477,970,703,372,813,691,659,414,442,262,111,365,979 };
std::vector<int> min_sets_ph = { 0,2,3,4,5,369,10,11,12,14,211,34,56,241,60,69,74,85,118,153,176,202,343,294,348,603,610,929,633,273,410,716,524,383,803,966,154,276,708 };
std::vector<int> min_sets_am = { 248,1,2,3,5,6,7,12,17,902,22,23,222,40,41,47,51,52,78,106,115,131,154,179,213,492,563,408,589,428,168,439,546,254,1000,740,189 };
std::vector<int> min_sets_dc = { 0,1,5,6,8,11,15,16,19,21,22,23,26,29,37,39,67,68,81,83,106,132,140,151,159,167,358,405,443,209,536,147,813,523,580,748,326,526,181,793,584,969,12,164 };
std::vector<int> min_sets_mh = { 0,1,2,3,4,5,7,10,17,21,219,29,39,42,65,67,77,91,146,156,181,276,393,450,917,550,175,780,484,281,953,95,989,457,629,481,571,882,766,412 };
std::vector<int> min_sets_dl = { 0,1,2,3,4,7,10,13,14,17,19,23,29,47,55,67,79,83,147,173,178,236,251,270,396,456,465,523,542,544,554,570,590,743,752,849,860,861,881,912,284,891 };
std::vector<int> min_sets_eq = { 0,1,2,3,5,6,7,15,17,23,26,30,32,38,52,59,72,79,99,137,206,234,301,307,395,412,846,401,405,922,240,663,204,804,787,565,432,765,424,398,678,571,732 };
std::vector	<int> min_sets_sh = { 0,1,2,4,5,10,15,16,18,20,22,24,25,28,32,35,41,43,50,53,64,87,95,126,200,225,321,555,1002,553,660,466,247,632,233,987,678,951,1004,757,157,819,557 };
std::vector<int> min_sets_ms = { 0,1,2,3,87,739,6,7,14,16,293,29,70,119,122,128,137,149,278,560,327,951,714,152,461,227,687,725,253,317,679,712,683,74,1019,112 };


int current_id = -1;

bool isSetFileModified(std::string set_fpath) {

	std::filesystem::file_time_type curr_modified_time = std::filesystem::last_write_time(set_fpath);
	if (last_setf_write != curr_modified_time) {
		last_setf_write = curr_modified_time;
		return true;
	}
	return false;
}



int chooseSet() {
	int set_num;
	if (shuffleSets) {
		PrintDebug("SHUFFLING ON");
		if (!selectWithoutReplacement) {
			PrintDebug("Selecting With Replacement");
			std::shuffle(setIDs.begin(), setIDs.end(), gen);
			set_num = setIDs.front();
			
		}
		else {
			PrintDebug("Selecting Without Replacement");
			if (setIDs.size() == setIDsCopy.size()) {
				std::shuffle(setIDs.begin(), setIDs.end(), gen);
			}
			set_num = setIDs.front();
			setIDs.erase(std::remove(setIDs.begin(), setIDs.end(), set_num), setIDs.end());
			if (setIDs.size() == 0) {
				PrintDebug("List Exhausted, Reloading");
				setIDs = setIDsCopy;
			}
		}
		std::ostringstream out;
		out << "RANDOM SET CHOSEN: " << set_num;
		
		//SendTimedDebugMessage(out.str(), 120);
		return set_num;
	}
	else {
		int set_num = setIDs.front();
		setIDs.erase(std::remove(setIDs.begin(), setIDs.end(), set_num), setIDs.end());
		if (setIDs.size() == 0) {
			setIDs = setIDsCopy;
		}

		return set_num;
	}
}
	

double getIGT() {
	double igt = TimerMinutes * 60.0 + TimerSeconds;
	double centis = ceil(TimerFrames/60.0*100);
	igt += centis / 100.0;
	return igt;
}

void logCurrentSet(int setID) {
	double igt = getIGT();
	PrintDebug("%d,%d,%f", CurrentLevel, setID, igt);
}

int chooseSetFromMinLists() {
	
		return chooseSet();
}

void __declspec(naked)hook1024() {
	__asm {
		pushad
	}
	if (MissionNum == 0 && useMinSets) {
		if (lastLevel != CurrentLevel) {
			switch (CurrentLevel) {
			case LevelIDs_WildCanyon:
				setIDs = min_sets_wc;
				break;
			case LevelIDs_PumpkinHill:
				setIDs = min_sets_ph;
				break;
			case LevelIDs_AquaticMine:
				setIDs = min_sets_am;
				break;
			case LevelIDs_DeathChamber:
				setIDs = min_sets_dc;
				break;
			case LevelIDs_MeteorHerd:
				setIDs = min_sets_mh;
				break;
			case LevelIDs_DryLagoon:
				setIDs = min_sets_dl;
				break;
			case LevelIDs_EggQuarters:
				setIDs = min_sets_eq;
				break;
			case LevelIDs_SecurityHall:
				setIDs = min_sets_sh;
				break;
			case LevelIDs_MadSpace:
				setIDs = min_sets_ms;
				break;
			default:
				PrintDebug("NOT A HUNTING STAGE");
				break;
			}
			PrintDebug("LOADING FOR LEVEL %d", CurrentLevel);
			lastLevel = CurrentLevel;
		}
		else {
			PrintDebug("STAYING ON LEVEL %d", CurrentLevel);
		}
		
		setIDsCopy = setIDs;
	}
	if (MissionNum == 0 && setIDs.size() > 0) {
		current_id = chooseSet();
		
		__asm {
			popad
			mov edi, current_id
			jmp hookExit
		}
	}
	else {	
		__asm {
			mov current_id, edi
			jmp hookExit
		}
	}
	__asm {
		popad
		jmp hookExit
	}
}

void __declspec(naked)hookResultsScreen() {
	__asm {
		pushad
	}
	if ((CurrentCharacter == Characters_Knuckles || CurrentCharacter == Characters_Rouge) && MissionNum == 0) {
		//log only deathless/restartless sets
		if (TimesRestartedOrDied == 0) {
			logCurrentSet(current_id);
		}
		curr_set_idx++;
		if (curr_set_idx >= setIDs.size()) {
			curr_set_idx = 0;
		}
		lastLevel = CurrentLevel;
		__asm {
			
			popad
			mov eax, 1
			mov ecx, 0x1A558A4
			mov [ecx], ax
			jmp resultsSkipExit
		}
	}
	curr_set_idx++;
	if (curr_set_idx >= setIDs.size()) {
		curr_set_idx = 0;
	}
	__asm {
		
		popad
		mov eax,1
		mov ecx,0x1A558A4
		mov [ecx],ax
		jmp resultsSkipExit
	}
}


void __declspec(naked)hookSecurityHallSafeColor() {
	__asm {
		pushad
	}
	if (!hasSetSafes) {
		CurrentSafeColor = safeColorIndex;
		int onSwitchPtr = safeSwitchArrayBase + safeColorIndex;
		if (safeColorIndex > 0) {
			__asm {
				and byte ptr [safeSwitchArrayBase], 0xFE
				or byte ptr [onSwitchPtr], 1
			}
		}
		hasSetSafes = true;
	}
	__asm {
		popad
		mov eax,[esi+0x30]
		mov ebx,[esi+0x34]
		jmp hookSafeColorExit
	}
}

void initHooks(bool shuffle, bool replacementSetting, bool useMin) {
	shuffleSets = shuffle;
	useMinSets = useMin;
	Hook((void*)hookEntryPt, hook1024, 6);
	Hook((void*)resultsEntryPt, hookResultsScreen, 5);
	//Hook((void*)hookSafeColorPt, hookSecurityHallSafeColor, 6);
	gen = std::mt19937(rd());
	selectWithoutReplacement = replacementSetting;
}


void StartTimeDB(std::string fn) {
	ildb_outf = new std::ofstream(fn);
	*ildb_outf << "Stage,ID,Time" << std::endl;
	
}



void LoadSetsFromFile(std::string fpath) {
	
	std::ifstream infile(fpath);
	int id;
	setIDs.clear();
	setIDsCopy.clear();
	PrintDebug("Sets Loaded:");
	while (infile >> id) {
		setIDs.push_back(id);
		setIDsCopy.push_back(id);
		PrintDebug("%d", id);	
	}
	
	infile.close();
	last_setf_write = std::filesystem::last_write_time(fpath);
}