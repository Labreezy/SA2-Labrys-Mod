#include "pch.h"
#include "emeraldset.h"
#include "asmutils.h"
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
int curr_set_idx = 0;

std::filesystem::file_time_type last_setf_write;
std::vector<int> setIDs;
std::vector<int> setIDsremaining;
static std::random_device rd;
static std::mt19937 gen;

int current_id = -1;

bool isSetFileModified(std::string set_fpath) {
	std::filesystem::file_time_type curr_modified_time = std::filesystem::last_write_time(set_fpath);
	if (last_setf_write != curr_modified_time) {
		last_setf_write = curr_modified_time;
		return true;
	}
	return false;
}

template<typename Iter, typename RandomGenerator>
Iter choose_random(Iter start, Iter end, RandomGenerator &g) {
	auto dist = std::distance(start, end);
	PrintDebug("Distance: %d", dist);
	std::uniform_int_distribution<> dis(0, setIDs.size() - 1);
	PrintDebug("Bounds: %d %d", dis.a(), dis.b());
	int n = dis(g);
	PrintDebug("Random choice: %d", n);
	std::advance(start, n);
	return start;
}

template<typename Iter>
Iter choose_random(Iter start, Iter end) {
	
		
	return choose_random(start, end, gen);
}
int chooseSet() {
	
		return *choose_random(setIDs.begin(), setIDs.end());
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
	*ildb_outf << CurrentLevel << "," << setID << "," << igt << std::endl;
}

void __declspec(naked)hook1024() {
	
	if (setIDs.size() > 0) {
		__asm {
			pushad
		}
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

void initHooks() {
	if (setIDs.size() > 0) {
		Hook((void*)hookEntryPt, hook1024, 6);
	}
	Hook((void*)resultsEntryPt, hookResultsScreen, 5);
	gen = std::mt19937(rd());
}


void StartTimeDB(std::string fn) {
	ildb_outf = new std::ofstream(fn);
	*ildb_outf << "Stage,ID,Time" << std::endl;
	
}

void LoadSetsFromFile(std::string fpath) {
	last_setf_write = std::filesystem::last_write_time(fpath);
	std::ifstream infile(fpath);
	int id;
	setIDs.clear();
	setIDsremaining.clear();
	PrintDebug("Sets Loaded:");
	while (infile >> id) {
		setIDs.push_back(id);
		setIDsremaining.push_back(id);
		PrintDebug("%d", id);	
	}
	
	infile.close();
}