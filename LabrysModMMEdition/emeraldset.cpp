#include "pch.h"
#include "emeraldset.h"
#include "asmutils.h"
#include <random>
#include <iterator>
#include <vector>
#include <fstream>
#include <sstream>

int hookEntryPt = 0x7380BF;
int resultsEntryPt = 0x451609;
DWORD hookExit = hookEntryPt + 6;
DWORD resultsSkipExit = resultsEntryPt + 5;
DWORD noScoreExit = 0x451535;
DWORD run_start_const = 0x174aff9;


std::vector<int> setIDs;
int current_id = -1;

template<typename Iter, typename RandomGenerator>
Iter choose_random(Iter start, Iter end, RandomGenerator &g) {
	std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
	std::advance(start, dis(g));
	return start;
}

template<typename Iter>
Iter choose_random(Iter start, Iter end) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return choose_random(start, end, gen);
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
		current_id = *choose_random(setIDs.begin(), setIDs.end());

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
		__asm {
			mov byte ptr [run_start_const], 0
			popad
			jmp noScoreExit
		}
	}
	__asm {
		mov byte ptr [run_start_const], 0
		popad
		jmp resultsSkipExit
	}
}

void initHooks() {
	if (setIDs.size() > 0) {
		Hook((void*)hookEntryPt, hook1024, 6);
	}
	Hook((void*)resultsEntryPt, hookResultsScreen, 5);
}


void StartTimeDB(std::string fn) {
	ildb_outf = new std::ofstream(fn);
	*ildb_outf << "Stage,ID,Time" << std::endl;
	
}

void LoadSetsFromFile(std::string fpath) {
	std::ifstream infile(fpath);
	int id;
	PrintDebug("Sets Loaded:");
	while (infile >> id) {
		setIDs.push_back(id);
		PrintDebug("%d", id);
	}
	infile.close();
}